from behave import *
from selenium.webdriver.common.by import By
import time


@given(u'administrator is loged in')
def step_impl(context):
    context.driver.get("http://opencart:8080/administration/")
    context.driver.find_element(By.ID, "input-username").click()
    context.driver.find_element(By.ID, "input-username").send_keys("user")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("bitnami")
    context.driver.find_element(By.CSS_SELECTOR, ".btn").click()


@given(u'administrator is on "Customers" page')
def step_impl(context):
    context.driver.find_element(By.LINK_TEXT, "Customers").click()
    context.driver.find_element(By.CSS_SELECTOR, "#collapse-5 > li:nth-child(1) > a").click()


@given(u'Customers List is not empty')
def step_impl(context):
    # done in test setup
    pass


@when(u'administrator marks certain customer in Customer List')
def step_impl(context):
    context.driver.find_element(By.NAME, "selected[]").click()


@when(u'clicks "Delete" button')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".fa-trash-can").click()
    assert context.driver.switch_to.alert.text == "Are you sure?"
    context.driver.switch_to.alert.accept()


@then(u'message indicating modification of Customer List is displayed')
def step_impl(context):
    assert context.driver.find_element(By.ID, "alert")


@then(u'deleted customer is not presented in Customer List')
def step_impl(context):
    time.sleep(1)
    assert context.driver.find_element(By.CSS_SELECTOR, ".col-sm-6:nth-child(2)").text == "Showing 1 to 2 of 2 (1 Pages)"
    context.driver.find_element(By.CSS_SELECTOR, ".navbar-brand > img").click()

###

@given(u'administrator is on "Edit Customer" page of certain customer')
def step_impl(context):
    context.driver.find_element(By.LINK_TEXT, "Customers").click()
    context.driver.find_element(By.CSS_SELECTOR, "#collapse-5 > li:nth-child(1) > a").click()
    context.driver.find_element(By.CSS_SELECTOR, "tr:nth-child(1) .fa-pencil").click()
  

@when(u'administrator changes Last Name about customer')
def step_impl(context):
    context.driver.find_element(By.ID, "input-lastname").click()
    context.driver.find_element(By.ID, "input-lastname").send_keys("5")


@when(u'clicks "Save" button')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary:nth-child(2)").click()


@then(u'message indicating successful modification is displayed')
def step_impl(context):
    assert context.driver.find_element(By.ID, "alert")


@then(u'data about this customer are changed')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".fa-reply").click()
    assert context.driver.find_element(By.CSS_SELECTOR, "tbody > tr:nth-child(1) > .text-start:nth-child(2)").text == "test 115"
    context.driver.find_element(By.CSS_SELECTOR, ".navbar-brand > img").click()


###

@given(u'Customer List is not empty')
def step_impl(context):
    # done in test setup
    pass


@when(u'administrator sets customer filter Customer Name')
def step_impl(context):
    context.driver.find_element(By.ID, "input-name").send_keys("test 12")


@when(u'clicks "Filter" button')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, "#button-filter > .fa-solid").click()


@then(u'customers that match the specified filter are displayed')
def step_impl(context):
    time.sleep(1)
    print(context.driver.find_element(By.CSS_SELECTOR, "tbody > tr:nth-child(1) > .text-start:nth-child(2)").text)
    assert context.driver.find_element(By.CSS_SELECTOR, "tbody > tr:nth-child(1) > .text-start:nth-child(2)").text == "test 12"


@then(u'customers that do not match the specified filter are not displayed')
def step_impl(context):
    print(context.driver.find_element(By.CSS_SELECTOR, ".col-sm-6:nth-child(2)").text)
    assert context.driver.find_element(By.CSS_SELECTOR, ".col-sm-6:nth-child(2)").text == "Showing 1 to 1 of 1 (1 Pages)"
    context.driver.find_element(By.CSS_SELECTOR, ".navbar-brand > img").click()

