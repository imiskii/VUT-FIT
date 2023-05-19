from behave import *
from selenium.webdriver.common.by import By
from selenium.webdriver.common.action_chains import ActionChains


@given(u'customer is on the "Regestration" page')
def step_impl(context):
    context.driver.get("http://opencart:8080/")
    context.driver.find_element(By.XPATH, "//nav[@id=\'top\']/div/div[2]/ul/li[2]/div/a/span").click()
    context.driver.find_element(By.LINK_TEXT, "Register").click()


@when(u'customer creates new account with his valid credentials')
def step_impl(context):
    context.driver.find_element(By.ID, "input-firstname").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("test")
    context.driver.find_element(By.ID, "input-lastname").click()
    context.driver.find_element(By.ID, "input-lastname").send_keys("3")
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys("test.3@mail.com")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("123456")
    context.driver.find_element(By.NAME, "agree").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()


@then(u'page with successful registration message is displayed')
def step_impl(context):
    assert context.driver.find_element(By.CSS_SELECTOR, "h1").text == "Register Account"
    context.driver.find_element(By.LINK_TEXT, "Continue").click()


@then(u'new customer with given credentials is presented in customer list')
def step_impl(context):
    context.driver.get("http://opencart:8080/administration/")
    context.driver.find_element(By.ID, "input-username").click()
    context.driver.find_element(By.ID, "input-username").send_keys("user")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("bitnami")
    context.driver.find_element(By.CSS_SELECTOR, ".btn").click()
    context.driver.find_element(By.LINK_TEXT, "Customers").click()
    context.driver.find_element(By.CSS_SELECTOR, "#collapse-5 > li:nth-child(1) > a").click()
    context.driver.find_element(By.CSS_SELECTOR, "tbody .text-start:nth-child(2)").click()
    context.driver.find_element(By.CSS_SELECTOR, "#nav-logout .d-none").click()

###

@given(u'customer is loged in his account')
def step_impl(context):
    context.driver.get("http://opencart:8080/")



@given(u'customer is on "Account" page')
def step_impl(context):
    context.driver.find_element(By.XPATH, "//nav[@id=\'top\']/div/div[2]/ul/li[2]/div/a/span").click()
    context.driver.find_element(By.CSS_SELECTOR, ".dropdown-menu-right > li:nth-child(1) > .dropdown-item").click()


@when(u'customer clicks on Logout button')
def step_impl(context):
    context.driver.find_element(By.LINK_TEXT, "Logout").click()


@then(u'Account Logout page is displayed')
def step_impl(context):
    assert context.driver.find_element(By.CSS_SELECTOR, "h1").text == "Account Logout"
    context.driver.find_element(By.LINK_TEXT, "Continue").click()


###

@when(u'customer tries to create an account with an email that is already used by another account')
def step_impl(context):
    context.driver.find_element(By.ID, "input-firstname").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("test")
    context.driver.find_element(By.ID, "input-lastname").click()
    context.driver.find_element(By.ID, "input-lastname").send_keys("4")
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys("test.3@mail.com")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("123456")
    context.driver.find_element(By.NAME, "agree").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()


@then(u'error message indicating that the email is already registered is displayed')
def step_impl(context):
    assert context.driver.find_element(By.CSS_SELECTOR, "h1").text == "Register Account"
    context.driver.find_element(By.CSS_SELECTOR, ".img-fluid").click()

###

@when(u'customer tries to create an account with inappropriate password')
def step_impl(context):
    context.driver.find_element(By.ID, "input-firstname").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("test")
    context.driver.find_element(By.ID, "input-lastname").click()
    context.driver.find_element(By.ID, "input-lastname").send_keys("4")
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys("test.4@mail.com")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("123")
    context.driver.find_element(By.NAME, "agree").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()


@then(u'error message indicating that password does not match the requirements is displayed')
def step_impl(context):
    context.driver.find_element(By.ID, "error-password").click()
    context.driver.find_element(By.CSS_SELECTOR, ".img-fluid").click()

###

@given(u'customer is on the login page')
def step_impl(context):
    context.driver.get("http://opencart:8080/")
    context.driver.find_element(By.XPATH, "//nav[@id=\'top\']/div/div[2]/ul/li[2]/div/a/span").click()
    context.driver.find_element(By.LINK_TEXT, "Login").click()


@when(u'customer log in with his valid credentials')
def step_impl(context):
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys("test.3@mail.com")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("123456")
    context.driver.find_element(By.CSS_SELECTOR, ".btn:nth-child(5)").click()


@then(u'page with user account is displayed')
def step_impl(context):
    assert context.driver.find_element(By.CSS_SELECTOR, "h2:nth-child(1)").text == "New Customer"
    context.driver.find_element(By.LINK_TEXT, "Logout").click()
    context.driver.find_element(By.LINK_TEXT, "Continue").click()

###

@when(u'customer tries to log in with invalid credentials')
def step_impl(context):
    context.driver.find_element(By.LINK_TEXT, "Login").click()
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys("test.3@mail.com")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("654321")
    context.driver.find_element(By.CSS_SELECTOR, ".btn:nth-child(5)").click()


@then(u'message indicating that incorrect log in credentials was provided is displayed and page is not changed')
def step_impl(context):
    assert context.driver.find_element(By.CSS_SELECTOR, "#form-login > h2").text == "Returning Customer"
    context.driver.find_element(By.CSS_SELECTOR, ".img-fluid").click()

