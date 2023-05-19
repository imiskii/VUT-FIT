#!/usr/bin/env python3
from selenium import webdriver
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities
from selenium.webdriver.common.by import By
from selenium.common.exceptions import WebDriverException
from behave import fixture
import sys
import time


def get_driver():
    '''Get Chrome/Firefox driver from Selenium Hub'''
    try:
        driver = webdriver.Remote(
                command_executor='http://localhost:4444/wd/hub',
                desired_capabilities=DesiredCapabilities.CHROME)
    except WebDriverException:
        driver = webdriver.Remote(
                command_executor='http://localhost:4444/wd/hub',
                desired_capabilities=DesiredCapabilities.FIREFOX)
    driver.implicitly_wait(15)

    return driver


def before_all(context):
    context.driver = get_driver()
    deleteAccounts(context)
    deleteOrders(context)
    

def after_all(context):
    context.driver.quit()


def before_feature(context, feature):
    if feature.name == "Shopping":
        try:
            setUpShopping(context)
        except Exception:
            for scenario in context.feature.scenarios:
                scenario.skip("Shopping feature set up failed")
    elif feature.name == "Customer Managment":
        try:
            setUpCustomerManagment(context)
        except Exception:
            for scenario in context.feature.scenarios:
                scenario.skip("Customer Managment feature set up failed")
    elif feature.name == "Order Management":
        try:
            setUpOrderManagment(context)
        except Exception:
            for scenario in context.feature.scenarios:
                scenario.skip("Order Management feature set up failed")


def after_feature(context, feature):
    if feature.name == "Shopping":
        try:
            cleanUpShopping(context)
        except Exception:
            sys.stderr.write("Error while Shopping feature cleanup\n")
            context.driver.quit()
            exit(1)
    elif feature.name == "Customer Account" or feature.name == "Customer Managment":
        try:
            deleteAccounts(context)
        except Exception:
            sys.stderr.write("Error while accounts clean up\n")
            context.driver.quit()
            exit(1)
    elif feature.name == "Order Management":
        try:
            deleteOrders(context)
        except Exception:
            sys.stderr.write("Error while orders clean up\n")
            context.driver.quit()
            exit(1)



# Set up tests for shopping
@fixture
def setUpShopping(context):
    context.driver.get("http://opencart:8080/")
    context.driver.find_element(By.XPATH, "//nav[@id=\'top\']/div/div[2]/ul/li[2]/div/a/span").click()
    context.driver.find_element(By.LINK_TEXT, "Register").click()
    context.driver.find_element(By.ID, "input-firstname").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("test")
    context.driver.find_element(By.ID, "input-lastname").click()
    context.driver.find_element(By.ID, "input-lastname").send_keys("1")
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys("test.1@mail.com")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("123456")
    context.driver.find_element(By.NAME, "agree").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()
    context.driver.find_element(By.LINK_TEXT, "Continue").click()
    context.driver.find_element(By.LINK_TEXT, "Address Book").click()
    context.driver.find_element(By.LINK_TEXT, "New Address").click()
    context.driver.find_element(By.ID, "input-firstname").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("test")
    context.driver.find_element(By.ID, "input-lastname").click()
    context.driver.find_element(By.ID, "input-lastname").send_keys("1")
    context.driver.find_element(By.ID, "input-address-1").click()
    context.driver.find_element(By.ID, "input-address-1").send_keys("home1")
    context.driver.find_element(By.ID, "input-city").click()
    context.driver.find_element(By.ID, "input-city").send_keys("LA")
    dropdown = context.driver.find_element(By.ID, "input-country")
    dropdown.find_element(By.XPATH, "//option[. = 'United States']").click()
    dropdown = context.driver.find_element(By.ID, "input-zone")
    dropdown.find_element(By.XPATH, "//option[. = 'California']").click()
    context.driver.execute_script("window.scrollTo(0, 1200);")
    time.sleep(1)
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()
    context.driver.execute_script("window.scrollTo(0, -1200);")
    time.sleep(1)
    context.driver.find_element(By.XPATH, "//nav[@id=\'top\']/div/div[2]/ul/li[2]/div/a/span").click()
    context.driver.find_element(By.LINK_TEXT, "Logout").click()
    context.driver.find_element(By.CSS_SELECTOR, ".img-fluid").click()


# Clean up after shopping tests
@fixture
def cleanUpShopping(context):
    context.driver.get("http://opencart:8080/administration/")
    context.driver.find_element(By.ID, "input-username").click()
    context.driver.find_element(By.ID, "input-username").send_keys("user")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("bitnami")
    context.driver.find_element(By.CSS_SELECTOR, ".btn").click()
    context.driver.find_element(By.LINK_TEXT, "Customers").click()
    context.driver.find_element(By.CSS_SELECTOR, "#collapse-5 > li:nth-child(1) > a").click()
    context.driver.find_element(By.NAME, "selected[]").click()
    context.driver.find_element(By.CSS_SELECTOR, "tr:nth-child(2) .form-check-input").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-danger").click()
    assert context.driver.switch_to.alert.text == "Are you sure?"
    context.driver.switch_to.alert.accept()
    context.driver.find_element(By.LINK_TEXT, "Sales").click()
    context.driver.find_element(By.LINK_TEXT, "Orders").click()
    context.driver.find_element(By.NAME, "selected[]").click()
    context.driver.find_element(By.CSS_SELECTOR, ".fa-trash-can").click()
    assert context.driver.switch_to.alert.text == "Are you sure?"
    context.driver.switch_to.alert.accept()
    context.driver.find_element(By.LINK_TEXT, "Logout").click()


# Set up tests for customer managment tests
@fixture
def setUpCustomerManagment(context):
    context.driver.get("http://opencart:8080/administration/")
    context.driver.find_element(By.ID, "input-username").click()
    context.driver.find_element(By.ID, "input-username").send_keys("user")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("bitnami")
    context.driver.find_element(By.CSS_SELECTOR, ".btn").click()
    context.driver.find_element(By.LINK_TEXT, "Customers").click()
    context.driver.find_element(By.CSS_SELECTOR, "#collapse-5 > li:nth-child(1) > a").click()
    for i in [10,11,12]:
        context.driver.find_element(By.CSS_SELECTOR, ".fa-plus").click()
        context.driver.find_element(By.ID, "input-firstname").click()
        context.driver.find_element(By.ID, "input-firstname").send_keys("test")
        context.driver.find_element(By.ID, "input-lastname").click()
        context.driver.find_element(By.ID, "input-lastname").send_keys(str(i))
        context.driver.find_element(By.ID, "input-email").click()
        context.driver.find_element(By.ID, "input-email").send_keys("test."+str(i)+"@mail.com")
        context.driver.execute_script("window.scrollTo(0, 1200);")
        time.sleep(1)
        context.driver.find_element(By.ID, "input-password").click()
        context.driver.find_element(By.ID, "input-password").send_keys("123456")
        context.driver.find_element(By.ID, "input-confirm").click()
        context.driver.find_element(By.ID, "input-confirm").send_keys("123456")
        context.driver.execute_script("window.scrollTo(0, -1200);")
        time.sleep(1)
        context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn-primary").click()
        context.driver.find_element(By.CSS_SELECTOR, ".btn-light").click()
        time.sleep(1)
    context.driver.find_element(By.CSS_SELECTOR, "#nav-logout .d-none").click()



# Set up tests for order managment tests
@fixture
def setUpOrderManagment(context):
    context.driver.get("http://opencart:8080/")
    for i in range(3):
        context.driver.find_element(By.CSS_SELECTOR, ".col:nth-child(2) .img-fluid").click()
        context.driver.find_element(By.ID, "button-cart").click()
        context.driver.find_element(By.LINK_TEXT, "Shopping Cart").click()
        context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn").click()
        context.driver.find_element(By.ID, "input-guest").click()
        if i == 0:
            context.driver.find_element(By.ID, "input-firstname").click()
            context.driver.find_element(By.ID, "input-firstname").send_keys("test")
            context.driver.find_element(By.ID, "input-lastname").click()
            context.driver.find_element(By.ID, "input-lastname").send_keys("5")
            context.driver.find_element(By.ID, "input-email").click()
            context.driver.find_element(By.ID, "input-email").send_keys("test.5@mail.com")
        context.driver.find_element(By.ID, "input-shipping-address-1").click()
        context.driver.find_element(By.ID, "input-shipping-address-1").send_keys("HOME")
        context.driver.find_element(By.ID, "input-shipping-city").click()
        context.driver.find_element(By.ID, "input-shipping-city").send_keys("LA")
        context.driver.execute_script("window.scrollTo(0, 1200);")
        time.sleep(1)
        dropdown = context.driver.find_element(By.ID, "input-shipping-country")
        dropdown.find_element(By.XPATH, "//option[. = 'United States']").click()
        dropdown = context.driver.find_element(By.ID, "input-shipping-zone")
        dropdown.find_element(By.XPATH, "//option[. = 'California']").click()
        context.driver.find_element(By.ID, "button-register").click()
        context.driver.execute_script("window.scrollTo(0, -1200);")
        time.sleep(1)
        dropdown = context.driver.find_element(By.ID, "input-shipping-method")
        dropdown.find_element(By.XPATH, "//option[. = 'Flat Shipping Rate - $5.00']").click()
        dropdown = context.driver.find_element(By.ID, "input-payment-method")
        dropdown.find_element(By.XPATH, "//option[. = 'Cash On Delivery']").click()
        context.driver.execute_script("window.scrollTo(0, 1200);")
        time.sleep(1)
        context.driver.find_element(By.ID, "button-confirm").click()
        context.driver.find_element(By.LINK_TEXT, "Continue").click()
        time.sleep(1)


# Delete all orders
@fixture
def deleteOrders(context):
    context.driver.get("http://opencart:8080/administration/")
    context.driver.find_element(By.ID, "input-username").click()
    context.driver.find_element(By.ID, "input-username").send_keys("user")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("bitnami")
    context.driver.find_element(By.CSS_SELECTOR, ".btn").click()
    context.driver.find_element(By.CSS_SELECTOR, "#menu-sale > .parent").click()
    context.driver.find_element(By.CSS_SELECTOR, "#collapse-4 > li:nth-child(1) > a").click()
    context.driver.find_element(By.CSS_SELECTOR, ".form-check-input").click()
    context.driver.find_element(By.ID, "button-delete").click()
    assert context.driver.switch_to.alert.text == "Are you sure?"
    context.driver.switch_to.alert.accept()
    context.driver.find_element(By.LINK_TEXT, "Logout").click()


# Delete all accounts
@fixture
def deleteAccounts(context):
    context.driver.get("http://opencart:8080/administration/")
    context.driver.find_element(By.ID, "input-username").click()
    context.driver.find_element(By.ID, "input-username").send_keys("user")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("bitnami")
    context.driver.find_element(By.CSS_SELECTOR, ".btn").click()
    context.driver.find_element(By.LINK_TEXT, "Customers").click()
    context.driver.find_element(By.CSS_SELECTOR, "#collapse-5 > li:nth-child(1) > a").click()
    context.driver.find_element(By.CSS_SELECTOR, ".form-check-input").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-danger").click()
    assert context.driver.switch_to.alert.text == "Are you sure?"
    context.driver.switch_to.alert.accept()
    context.driver.find_element(By.CSS_SELECTOR, "#nav-logout .d-none").click()
