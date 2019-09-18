package camera

import (
	"fmt"
	"log"
	"path/filepath"
	"strings"
	"sync"
	"time"
	"image"
	"image/color"
	"image/draw"
	"image/jpeg"
	"io/ioutil"
	"log"
	"os"
	"unicode/utf8"

	"github.com/disintegration/imaging"
	"github.com/golang/freetype"
	"github.com/reef-pi/reef-pi/controller"
	"github.com/reef-pi/reef-pi/controller/storage"
	"github.com/reef-pi/reef-pi/controller/utils"
)

const Bucket = storage.CameraBucket
const ItemBucket = storage.CameraItemBucket

type Controller struct {
	config  Config
	stopCh  chan struct{}
	mu      sync.Mutex
	DevMode bool
	c       controller.Controller
}

func New(devMode bool, c controller.Controller) (*Controller, error) {
	return &Controller{
		config:  Default,
		mu:      sync.Mutex{},
		DevMode: devMode,
		stopCh:  make(chan struct{}),
		c:       c,
	}, nil
}

func (c *Controller) On(id string, b bool) error {
	return fmt.Errorf("Camera subsystem does not support 'on' interface")
}

func (c *Controller) Start() {
	go c.runPeriodically()
}

func (c *Controller) run() {
	if !c.config.Enable {
		return
	}
	img, err := c.Capture()
	if err != nil {
		log.Println("ERROR: camera subsystem: failed to capture image. Error:", err)
		c.c.LogError("camera-capture", "Failed to capture image. Error:"+err.Error())
		return
	}
	if err := c.Process(img); err != nil {
		log.Println("ERROR: camera sub-system : Failed to process image. Error:", err)
		c.c.LogError("camera-process", "Failed to process image. Error:"+err.Error())
	}
	if c.config.Upload {
		c.uploadImage(img)
	}
}

func (c *Controller) runPeriodically() {
	log.Println("Starting camera controller")
	ticker := time.NewTicker(c.config.TickInterval * time.Minute)
	for {
		select {
		case <-ticker.C:
			c.run()
		case <-c.stopCh:
			log.Println("Stopping camera controller")
			ticker.Stop()
			return
		}
	}
}

func (c *Controller) Stop() {
	c.stopCh <- struct{}{}
}

func (c *Controller) Setup() error {
	if err := c.c.Store().CreateBucket(Bucket); err != nil {
		return err
	}
	conf, err := loadConfig(c.c.Store())
	if err != nil {
		log.Println("WARNING: camera config not found. Initializing default config")
		conf = Default
		if err := saveConfig(c.c.Store(), conf); err != nil {
			log.Println("ERROR: Failed to save camera config. Error:", err)
			return err
		}
	}
	c.mu.Lock()
	c.config = conf
	c.mu.Unlock()
	if err := c.c.Store().CreateBucket(ItemBucket); err != nil {
		return err
	}
	return nil
}

func (c *Controller) Capture() (string, error) {
	c.mu.Lock()
	defer c.mu.Unlock()
	imgDir, pathErr := filepath.Abs(c.config.ImageDirectory)
	if pathErr != nil {
		return "", pathErr
	}
	imgName := time.Now().Format("15-04-05-Mon-Jan-2-2006.png")
	imgPath := filepath.Join(imgDir, imgName)
	command := "raspistill -e png " + c.config.CaptureFlags + " -o " + imgPath
	parts := strings.Fields(command)
	err := utils.Command(parts[0], parts[1:]...).WithDevMode(c.DevMode).Run()
	if err != nil {
		log.Println("ERROR: Failed to execute image capture command:", command, "Error:", err)
		return "", err
	}
	data := make(map[string]string)
	data["image"] = imgName
	log.Println("Camera subsystem: Image captured:", imgPath)
	return imgName, c.c.Store().Update(Bucket, "latest", data)
}

func (c *Controller) uploadImage(imgName string) {
	imgDir, pathErr := filepath.Abs(c.config.ImageDirectory)
	if pathErr != nil {
		log.Println("ERROR: Failed to compute absolute image path. Error:", pathErr)
		return
	}
	imgPath := filepath.Join(imgDir, imgName)
	command := "drive push -quiet -destination reef-pi-images -files " + imgPath
	parts := strings.Fields(command)
	err := utils.Command(parts[0], parts[1:]...).WithDevMode(c.DevMode).Run()
	if err != nil {
		log.Println("ERROR: Failed to upload image. Command:", command, "Error:", err)
	}
}

func (c *Controller) watermark() {
	srcImg, err := imaging.Open(imgPath)
	if err != nil {
		log.Fatal(err)
	}

	//workpath := time.Now().Unix()

	text := fmt.Sprintf(imgPath, time.Now().Format("15-04-05-Mon-Jan-2-2006"))
	textImg := makeTextImage(text, 32, 60)

	imaging.Save(textImg, "text_img.png")

	offset := image.Pt(srcImg.Bounds().Dx()-textImg.Bounds().Dx(), srcImg.Bounds().Dy()-textImg.Bounds().Dy())

	newImg := image.NewRGBA(srcImg.Bounds())
	draw.Draw(newImg, srcImg.Bounds(), srcImg, image.ZP, draw.Src)
	draw.Draw(newImg, textImg.Bounds().Add(offset), textImg, image.ZP, draw.Over)

	output, err := os.Create("watermark.jpg")
	//output, err := os.Create(fmt.Sprintf("watermark_%d.jpg", timestamp))
	if err != nil {
		log.Fatal(err)
	}

	defer output.Close()
	err = jpeg.Encode(output, newImg, &jpeg.Options{Quality: 100})
	if err != nil {
		log.Fatal(err)
	}
	return
}

func (c *Controller) makeTextImage(text string, fontsize, rotate float64) *image.NRGBA {
	fontBytes, err := ioutil.ReadFile("msyh.ttc")
	if err != nil {
		log.Println(err)
	}

	font, err := freetype.ParseFont(fontBytes)
	if err != nil {
		log.Println(err)
	}

	ctx := freetype.NewContext()
	ctx.SetDPI(72)
	ctx.SetFont(font)
	ctx.SetFontSize(fontsize)
	ctx.SetSrc(image.NewUniform(color.RGBA{R: 0, G: 0, B: 0, A: 255}))

	txtImg := image.NewNRGBA(image.Rect(0, 0, int(fontsize)*utf8.RuneCountInString(text), int(fontsize)))
	ctx.SetClip(txtImg.Bounds())
	ctx.SetDst(txtImg)

	pt := freetype.Pt(0, int(-fontsize/6)+ctx.PointToFixed(fontsize).Ceil())
	ctx.DrawString(text, pt)

	if rotate > 0 {
		return imaging.Rotate(txtImg, rotate, color.Transparent)
	} else {
		return txtImg
	}
}
