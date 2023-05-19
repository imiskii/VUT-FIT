Feature: Shopping

    Scenario: Empty Cart
        Given the shopping cart is empty
        When customer clicks on "Shopping Cart" button
        Then page with shopping cart is displayed
        And there is no product in shopping cart


    Scenario Outline: Search Product
        Given there are products available
        When customer searchs for some <term> in the search bar
        Then products related to searched term are displayed

        Examples:
            | term   |
            | iPhone |


    Scenario: Add Product To Cart
        Given customer is on page with certain product
        When customer clicks "Add to Cart" button
        Then message indicating that new product is added to shopping cart is displayed
        And product is visible in shopping cart


    Scenario Outline: Change Quantity Of Product In Cart
        Given the shopping cart is not empty
        And customer is on "Shopping Cart" page
        When customer enters <number> in to "Quantity" bar of certain product
        And click the "Update" button
        Then message indicating that shopping cart is successfully modified is displayed
        And cost of the products is updated

        Examples:
            | number |
            | 10     |


    Scenario: Remove Product From Cart
        Given the shopping cart is not empty
        And customer is on "Shopping Cart" page
        When customer click on "Remove" button of certain product in shopping cart
        Then this product is no longer visible in shopping cart


    Scenario: Open Checkout Page
        Given the shopping cart is not empty
        And customer is on "Shopping Cart" page
        When the "Checkout" button is clicked
        Then the "Checkout" page is opened


    Scenario: Buying Product
        Given customer is loged in
        And customer is on "Checkout" page of specific product
        When customer fills in all the necessary data for the order
        And click "Confirm Order" button
        Then page with successful order message is displayed
        And customer can see new order in his Order History list


    Scenario: Register Account During Checkout
        Given customer is on "Checkout" page
        And customer has no account created
        And customer wants to register new account
        When customer fills in all the required data with his valid credentials
        And click "Continue" button
        Then message indicating successful account creation is displayed

