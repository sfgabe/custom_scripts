## updated for python3 from https://github.com/daniyalzade/coned-parser

from collections import OrderedDict
import re
from time import sleep

from bs4 import BeautifulSoup
import configargparse

import utils
from utils import get_account_page_driver
from utils import parse_price

DEBUG_FILE = 'tmp/coned.html'
IMAGE_FILE = 'tmp/screen.png'
LOGIN_CONFIG = {
    'url': 'https://www.coned.com/en/login',
    'username_xpath': '//input[@id=\'form-login-email\']',
    'password_xpath': '//input[@id=\'form-login-password\']',
    'submit_xpath': '//button[contains(@class, \'js-login-submit-button\')]',
    'dom_xpath': '//img[@id=\'ctl00_imgApplicationName\']'
}
REGEX_DATE = r'(\d+/\d+/\d+)'
REGEX_AMOUNT = r'\$(\d+.\d+)'

def _get_account_page(username, password, debug=False):
    config = dict(LOGIN_CONFIG,
                  username=username,
                  password=password)
    driver = get_account_page_driver(config)
    if debug:
        html_source = driver.page_source.encode('utf8')
        open(DEBUG_FILE, "w").write(html_source)
        driver.save_screenshot(IMAGE_FILE)
    return driver


def _parse_bill_text(text):
    try:
        due = re.search(REGEX_DATE, text).group(1)
        amount = parse_price(re.search(REGEX_AMOUNT, text).group(1))
        return (due, amount)
    except Exception:
        return (None, None)


def _get_bills_page(driver):
    driver.find_element_by_xpath("//a[@id='ctl00_Main_hpLblBillingHistory']").click()
    print ('sleeping to get the new page')
    sleep(10)
    return driver


def _parse_bills(html):
    soup = BeautifulSoup(html, 'html.parser')
    bills = []
    for row in soup.find(id='ctl00_Main_lvBillHistory_Table1').find_all('tr')[2:]:
        children = row.find_all('td')
        bill = OrderedDict()
        bill['start'] = children[0].text.strip()
        bill['end'] = children[1].text.strip()
        bill['usage'] = children[2].text.strip()
        bill['amount'] = parse_price(children[4].text)
        bills.append(bill)
    return bills


def _parse_current_bill(html):
    soup = BeautifulSoup(html, 'html.parser')
    prev, cur = soup.find(id='divAccountBalance').text.lower().split('current')
    prev_due, prev_amount = _parse_bill_text(prev)
    cur_due, cur_amount = _parse_bill_text(cur)
    return [{
        'amount': prev_amount,
        'due': prev_due,
        'current': False,
    }, {
        'amount': cur_amount,
        'due': cur_due,
        'current': True,
    }]


def main():
    p = configargparse.ArgParser(default_config_files=['.credentials'])
    p.add('--username', required=True, help='username')
    p.add('--password', required=True, help='password')
    p.add('--debug', help='debug', action='store_true')

    options = p.parse_args()
    driver = _get_account_page(options.username, options.password,
                            debug=options.debug)
    html = driver.page_source.encode('utf8')
    print (_parse_current_bill(html))
    driver = _get_bills_page(driver)
    html = driver.page_source.encode('utf8')
    print (utils.print_bills_as_csv(_parse_bills(html)))


if __name__ == "__main__":
    main()
