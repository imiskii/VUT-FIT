from behave import *
from selenium.webdriver.common.by import By
import time


@given(u'administrator is on Add Product page')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, "#menu-catalog > .parent").click()
    context.driver.find_element(By.CSS_SELECTOR, "#collapse-1 > li:nth-child(2) > a").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn > .fa-plus").click()


@when(u'administrator fills all necessary data for new product')
def step_impl(context):
    context.driver.find_element(By.ID, "input-name-1").click()
    context.driver.find_element(By.ID, "input-name-1").send_keys("AAAproduct1")
    context.driver.find_element(By.ID, "input-meta-title-1").click()
    context.driver.find_element(By.ID, "input-meta-title-1").send_keys("ppp")
    context.driver.find_element(By.LINK_TEXT, "Data").click()
    context.driver.find_element(By.ID, "input-model").click()
    context.driver.find_element(By.ID, "input-model").send_keys("x20")
    context.driver.find_element(By.LINK_TEXT, "SEO").click()
    context.driver.find_element(By.ID, "input-keyword-0-1").click()
    context.driver.find_element(By.ID, "input-keyword-0-1").send_keys("x20")


@when(u'clicks "Save" product button')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".fa-floppy-disk").click()


@then(u'message indicating successful product modification is displayed')
def step_impl(context):
    time.sleep(1)
    assert context.driver.find_element(By.ID, "alert")
    context.driver.find_element(By.CSS_SELECTOR, ".navbar-brand > img").click()


###

@given(u'administrator is on "Products" page')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, "#menu-catalog > .parent").click()
    context.driver.find_element(By.CSS_SELECTOR, "#collapse-1 > li:nth-child(2) > a").click()


@when(u'administrator sets product filter Product Name')
def step_impl(context):
    context.driver.find_element(By.ID, "input-name").send_keys("AAAproduct1")
    

@when(u'click "Filter" button')
def step_impl(context):
    context.driver.find_element(By.ID, "button-filter").click()


@then(u'products that match the specified filter are displayed')
def step_impl(context):
    time.sleep(1)
    print(context.driver.find_element(By.CSS_SELECTOR, "tbody .text-start:nth-child(3)").text)
    assert context.driver.find_element(By.CSS_SELECTOR, "tbody .text-start:nth-child(3)").text == "AAAproduct1\nEnabled"


@then(u'products that do not match the specified filter are not displayed')
def step_impl(context):
    print(context.driver.find_element(By.CSS_SELECTOR, ".col-sm-6:nth-child(2)").text)
    assert context.driver.find_element(By.CSS_SELECTOR, ".col-sm-6:nth-child(2)").text == "Showing 1 to 1 of 1 (1 Pages)"
    context.driver.find_element(By.CSS_SELECTOR, ".navbar-brand > img").click()

###

@given(u'administrator is on Edit Product page of certain product')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, "#menu-catalog > .parent").click()
    context.driver.find_element(By.CSS_SELECTOR, "#collapse-1 > li:nth-child(2) > a").click()
    time.sleep(1)
    context.driver.find_element(By.CSS_SELECTOR, "tr:nth-child(1) .fa-pencil").click()
    context.driver.find_element(By.LINK_TEXT, "Data").click()


@when(u'administrator changes data about product')
def step_impl(context):
    context.driver.find_element(By.ID, "input-model").click()
    context.driver.find_element(By.ID, "input-model").send_keys("xxx")


@when(u'click "Save" changes button')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn-primary").click()
    context.driver.find_element(By.CSS_SELECTOR, ".fa-reply").click()


@then(u'changed product data are applied')
def step_impl(context):
    time.sleep(1)
    assert context.driver.find_element(By.ID, "alert")
    assert context.driver.find_element(By.CSS_SELECTOR, "tbody > tr:nth-child(1) > .d-none").text == "x20xxx"
    context.driver.find_element(By.CSS_SELECTOR, ".navbar-brand > img").click()

###


@given(u'Product List is not empty')
def step_impl(context):
    # By dafault
    pass


@when(u'administrator marks product in Product List')
def step_impl(context):
    context.driver.find_element(By.NAME, "selected[]").click()


@when(u'click "Delete" button')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".fa-trash-can").click()
    assert context.driver.switch_to.alert.text == "Are you sure?"
    context.driver.switch_to.alert.accept()

