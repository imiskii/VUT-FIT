from behave import *
from selenium.webdriver.common.by import By
import time



@given(u'administrator is on "Orders" page')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, "#menu-sale > .parent").click()
    context.driver.find_element(By.CSS_SELECTOR, "#collapse-4 > li:nth-child(1) > a").click()


@given(u'Orders List is not empty')
def step_impl(context):
    # default by set up
    pass


@when(u'administrator marks certain order')
def step_impl(context):
    context.driver.find_element(By.NAME, "selected[]").click()


@when(u'clicks order "Delete" button')
def step_impl(context):
    context.driver.find_element(By.ID, "button-delete").click()
    assert context.driver.switch_to.alert.text == "Are you sure?"
    context.driver.switch_to.alert.accept()


@then(u'message indicating modification of Orders List is displayed')
def step_impl(context):
    assert context.driver.find_element(By.ID, "alert")


@then(u'deleted order is not presented in the Orders List')
def step_impl(context):
    time.sleep(1)
    print(context.driver.find_element(By.CSS_SELECTOR, ".col-sm-6:nth-child(2)").text)
    assert context.driver.find_element(By.CSS_SELECTOR, ".col-sm-6:nth-child(2)").text == "Showing 1 to 2 of 2 (1 Pages)"
    context.driver.find_element(By.CSS_SELECTOR, ".navbar-brand > img").click()

###

@given(u'administrator is on certain order edit page')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, "#menu-sale > .parent").click()
    context.driver.find_element(By.CSS_SELECTOR, "#collapse-4 > li:nth-child(1) > a").click()
    context.driver.find_element(By.CSS_SELECTOR, "tr:nth-child(1) .fa-solid").click()


@when(u'administrator changes order status to Shipped')
def step_impl(context):
    context.driver.execute_script("window.scrollTo(0, 1500);")
    time.sleep(1)
    dropdown = context.driver.find_element(By.ID, "input-order-status")
    dropdown.find_element(By.XPATH, "//option[. = 'Shipped']").click()


@when(u'clicks the "Add History" button')
def step_impl(context):
    context.driver.find_element(By.ID, "button-history").click()


@then(u'message indicating modification of order is displayed')
def step_impl(context):
    assert context.driver.find_element(By.ID, "alert")


@then(u'selected status with current date is shown in History of statuses of current order')
def step_impl(context):
    time.sleep(1)
    assert context.driver.find_element(By.CSS_SELECTOR, "tbody > tr:nth-child(1) > .text-start:nth-child(3)").text == "Shipped"
    context.driver.execute_script("window.scrollTo(0, -1500);")
    time.sleep(1)
    context.driver.find_element(By.CSS_SELECTOR, ".navbar-brand > img").click()


###

@given(u'order list is not empty')
def step_impl(context):
    # default by set up
    pass


@when(u'administrator sets order filter Order Status')
def step_impl(context):
    dropdown = context.driver.find_element(By.ID, "input-order-status")
    dropdown.find_element(By.XPATH, "//option[. = 'Shipped']").click()


@when(u'clicks order "Filter" button')
def step_impl(context):
    context.driver.find_element(By.ID, "button-filter").click()


@then(u'orders that match the specified filter are displayed')
def step_impl(context):
    time.sleep(1)
    assert context.driver.find_element(By.CSS_SELECTOR, ".text-start > label").text == "Shipped"


@then(u'orders that do not match the specified filter are not displayed')
def step_impl(context):
    assert context.driver.find_element(By.CSS_SELECTOR, ".col-sm-6:nth-child(2)").text == "Showing 1 to 1 of 1 (1 Pages)"
    context.driver.find_element(By.CSS_SELECTOR, ".navbar-brand > img").click()

