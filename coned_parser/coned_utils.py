## Updated for Python3 from https://github.com/daniyalzade/coned-parser
## Removed depreciated PhantamJS requirement, replaced with chromedriver

import re
from selenium import webdriver
from selenium.common.exceptions import NoSuchElementException
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.chrome.options import Options

def parse_price(price, fun=max):
    """
    @param price: str
    @param fun: max | min : determine max or min of price range when there is
    conflicts
    @return: float
    taking
    """
    price = price.lower().replace('for', '|')
    price = re.sub(r'[\(\)a-z\s$&;:]', '', price.lower())
    # Strip leading / trailing .'s
    price = price.strip('.')
    # Combine comma-separation used for denoting 000s
    if '|' in price:
        quantity, amount = price.split('|')
        return float(amount) / float(quantity)
    return float(fun(price.replace(',', '').split('-')))  # '$308.00 - $440.00'


def get_account_page_driver(login_config):

    chrome_options = Options()
    chrome_options.add_argument("headless")
    chrome_options.add_argument("window-size=1024,768")

    driver = webdriver.Chrome('/usr/lib/chromium-browser/chromedriver')

    driver.get(login_config['url'])
    element = driver.find_element_by_xpath(login_config['username_xpath'])
    element.send_keys(login_config['username'])
    element = driver.find_element_by_xpath(login_config['password_xpath'])
    element.send_keys(login_config['password'])
    driver.find_element_by_xpath(login_config['submit_xpath']).submit()

    dom_xpath = login_config.get('dom_xpath')
    if dom_xpath:
        print ('waiting for the appearence of the required element')
        element = WebDriverWait(driver, 50).until(
            EC.presence_of_element_located((By.XPATH, dom_xpath))
        )
        login_error_xpath = login_config.get('login_error')
    if not login_error_xpath:
        return driver
    try:
        element = driver.find_element_by_xpath(login_config['login_error'])
        raise Exception('invalid login credentials')
    # No login error encountered
    except NoSuchElementException:
        return driver
    return driver

def print_bills_as_csv(bills):
    for bill in bills:
        print (', '.join(map(str, bill.values())))
