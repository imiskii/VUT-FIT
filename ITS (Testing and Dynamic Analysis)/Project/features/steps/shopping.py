from behave import *
from selenium.webdriver.common.by import By
import re
import time


@given(u'the shopping cart is empty')
def step_impl(context):
    # By default is empty
    pass


@when(u'customer clicks on "Shopping Cart" button')
def step_impl(context):
    context.driver.get("http://opencart:8080/")
    context.driver.find_element(By.CSS_SELECTOR, ".btn-inverse").click()


@then(u'page with shopping cart is displayed')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, "li > .text-center") == "Your shopping cart is empty!"


@then(u'there is no product in shopping cart')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".list-inline-item:nth-child(4) .d-none").click()
    assert context.driver.find_element(By.CSS_SELECTOR, "p:nth-child(2)").text == "Your shopping cart is empty!"
    # cleanup
    context.driver.find_element(By.LINK_TEXT, "Continue").click()
    
###

@given(u'there are products available')
def step_impl(context):
    # They are available by default
    context.driver.get("http://opencart:8080/")


@when(u'customer searchs for some iPhone in the search bar')
def step_impl(context):
    context.driver.find_element(By.NAME, "search").click()
    context.driver.find_element(By.NAME, "search").send_keys("iPhone")
    context.driver.find_element(By.CSS_SELECTOR, ".fa-magnifying-glass").click()


@then(u'products related to searched term are displayed')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".image .img-fluid").click() 
    assert re.match(r".*(iPhone).*", context.driver.find_element(By.CSS_SELECTOR, "h1").text, re.IGNORECASE) != None


###

@given(u'customer is on page with certain product')
def step_impl(context):
    context.driver.get("http://opencart:8080/en-gb/product/iphone")


@when(u'customer clicks "Add to Cart" button')
def step_impl(context):
    context.driver.find_element(By.ID, "button-cart").click()


@then(u'message indicating that new product is added to shopping cart is displayed')
def step_impl(context):
    assert context.driver.find_element(By.ID, "alert")


@then(u'product is visible in shopping cart')
def step_impl(context):
    context.driver.find_element(By.LINK_TEXT, "Shopping Cart").click()
    context.driver.find_element(By.LINK_TEXT, "iPhone").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-inverse").click()
    context.driver.find_element(By.LINK_TEXT, "iPhone").click()
    # cleanup
    context.driver.find_element(By.LINK_TEXT, "Shopping Cart").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn:nth-child(4) > .fa-solid").click()



###

@given(u'the shopping cart is not empty')
def step_impl(context):
    time.sleep(1)
    context.driver.get("http://opencart:8080/en-gb/product/iphone")
    context.driver.find_element(By.ID, "button-cart").click()


@given(u'customer is on "Shopping Cart" page')
def step_impl(context):
    context.driver.get("http://opencart:8080/en-gb?route=checkout/cart")


@when(u'customer enters 10 in to "Quantity" bar of certain product')
def step_impl(context):
    context.driver.find_element(By.NAME, "quantity").click()
    context.driver.find_element(By.NAME, "quantity").send_keys("0")


@when(u'click the "Update" button')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".fa-rotate").click()


@then(u'message indicating that shopping cart is successfully modified is displayed')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".nav:nth-child(2)").click()


@then(u'quantity of this product is updated')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".btn-inverse").click()
    assert context.driver.find_element(By.CSS_SELECTOR, ".text-end:nth-child(3)").text == "x 10"


@then(u'cost of the products is updated')
def step_impl(context):
    pece = context.driver.find_element(By.CSS_SELECTOR, "tbody:nth-child(2) .text-end:nth-child(5)").text
    sum = context.driver.find_element(By.CSS_SELECTOR, "tbody .text-end:nth-child(6)").text
    pece = float(pece.replace(',', '')[1:])
    sum = float(sum.replace(',', '')[1:])
    assert sum == round(pece*10, 2)
    # clean up
    context.driver.find_element(By.CSS_SELECTOR, ".btn:nth-child(4) > .fa-solid").click()


###

@when(u'customer click on "Remove" button of certain product in shopping cart')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".btn:nth-child(4) > .fa-solid").click()
    

@then(u'this product is no longer visible in shopping cart')
def step_impl(context):
    time.sleep(1)
    assert context.driver.find_element(By.CSS_SELECTOR, "p:nth-child(2)").text == "Your shopping cart is empty!"
    context.driver.find_element(By.LINK_TEXT, "Continue").click()

###

@when(u'the "Checkout" button is clicked')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn").click()


@then(u'the "Checkout" page is opened')
def step_impl(context):
    assert context.driver.find_element(By.CSS_SELECTOR, "h1").text == "Checkout"

###

@given(u'customer is loged in')
def step_impl(context):
    context.driver.get("http://opencart:8080/")
    context.driver.find_element(By.CSS_SELECTOR, ".col:nth-child(2) .img-fluid").click()
    context.driver.find_element(By.ID, "button-cart").click()
    context.driver.find_element(By.XPATH, "//nav[@id=\'top\']/div/div[2]/ul/li[2]/div/a/span").click()
    context.driver.find_element(By.LINK_TEXT, "Login").click()
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys("test.1@mail.com")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("123456")
    context.driver.find_element(By.CSS_SELECTOR, ".btn:nth-child(5)").click()
    context.driver.find_element(By.CSS_SELECTOR, "#logo .img-fluid").click()


@given(u'customer is on "Checkout" page of specific product')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".list-inline-item:nth-child(4) .d-none").click()
    context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn").click()


@when(u'customer fills in all the necessary data for the order')
def step_impl(context):
    dropdown = context.driver.find_element(By.ID, "input-shipping-address")
    dropdown.find_element(By.XPATH, "//option[. = 'test 1, home1, LA, California, United States']").click()
    dropdown = context.driver.find_element(By.ID, "input-shipping-method")
    dropdown.find_element(By.XPATH, "//option[. = 'Flat Shipping Rate - $5.00']").click()
    dropdown = context.driver.find_element(By.ID, "input-payment-method")
    dropdown.find_element(By.XPATH, "//option[. = 'Cash On Delivery']").click()


@when(u'click "Confirm Order" button')
def step_impl(context):
    context.driver.execute_script("window.scrollTo(0, 1200);")
    time.sleep(1)
    context.driver.find_element(By.ID, "button-confirm").click()


@then(u'page with successful order message is displayed')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, "#content > p:nth-child(3)").click()
    context.driver.find_element(By.LINK_TEXT, "Continue").click()


@then(u'customer can see new order in his Order History list')
def step_impl(context):
    context.driver.find_element(By.XPATH, "//nav[@id=\'top\']/div/div[2]/ul/li[2]/div/a/span").click()
    context.driver.find_element(By.LINK_TEXT, "Order History").click()
    context.driver.find_element(By.CSS_SELECTOR, "tbody .text-end:nth-child(1)").click()
    context.driver.find_element(By.LINK_TEXT, "Continue").click()
    # cleanup
    context.driver.find_element(By.LINK_TEXT, "Logout").click()
    context.driver.find_element(By.LINK_TEXT, "Continue").click()

###

@given(u'customer is on "Checkout" page')
def step_impl(context):
    context.driver.get("http://opencart:8080/en-gb/product/iphone")
    context.driver.find_element(By.ID, "button-cart").click()
    context.driver.find_element(By.CSS_SELECTOR, "a > .fa-cart-shopping").click()
    context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn").click()


@given(u'customer has no account created')
def step_impl(context):
    # By default
    pass


@given(u'customer wants to register new account')
def step_impl(context):
    context.driver.find_element(By.ID, "input-register").click()


@when(u'customer fills in all the required data with his valid credentials')
def step_impl(context):
    context.driver.find_element(By.ID, "input-firstname").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("test")
    context.driver.find_element(By.ID, "input-lastname").click()
    context.driver.find_element(By.ID, "input-lastname").send_keys("2")
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys("test.2@mail.com")
    context.driver.find_element(By.ID, "input-shipping-address-1").click()
    context.driver.find_element(By.ID, "input-shipping-address-1").send_keys("HOME1")
    context.driver.find_element(By.ID, "input-shipping-city").click()
    context.driver.find_element(By.ID, "input-shipping-city").send_keys("LA")
    dropdown = context.driver.find_element(By.ID, "input-shipping-country")
    dropdown.find_element(By.XPATH, "//option[. = 'United States']").click()
    dropdown = context.driver.find_element(By.ID, "input-shipping-zone")
    dropdown.find_element(By.XPATH, "//option[. = 'California']").click()
    context.driver.execute_script("window.scrollTo(0, 1200);")
    time.sleep(1)
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("123456")
    context.driver.find_element(By.ID, "input-register-agree").click()


@when(u'click "Continue" button')
def step_impl(context):
    context.driver.find_element(By.ID, "button-register").click()


@then(u'message indicating successful account creation is displayed')
def step_impl(context):
    context.driver.execute_script("window.scrollTo(0, -1200);")
    time.sleep(1)
    context.driver.find_element(By.CSS_SELECTOR, ".img-fluid").click()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-inverse").click()
    context.driver.find_element(By.CSS_SELECTOR, ".fa-circle-xmark").click()
    time.sleep(1)
    context.driver.find_element(By.CSS_SELECTOR, ".img-fluid").click()
    context.driver.find_element(By.XPATH, "//nav[@id=\'top\']/div/div[2]/ul/li[2]/div/a/span").click()
    context.driver.find_element(By.LINK_TEXT, "Logout").click()
    context.driver.find_element(By.CSS_SELECTOR, ".img-fluid").click()
