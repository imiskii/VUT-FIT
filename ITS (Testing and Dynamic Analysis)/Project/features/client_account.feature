Feature: Customer Account

    Scenario: Successful Registration
        Given customer is on the "Regestration" page
        When customer creates new account with his valid credentials
        Then page with successful registration message is displayed
        And new customer with given credentials is presented in customer list
    
    
    Scenario: Logout From Customer Account
        Given customer is loged in his account
        And customer is on "Account" page
        When customer clicks on Logout button
        Then Account Logout page is displayed


    Scenario: Registration With Existing Email
        Given customer is on the "Regestration" page
        When customer tries to create an account with an email that is already used by another account
        Then error message indicating that the email is already registered is displayed


    Scenario: Registration With Inappropriate Password
        Given customer is on the "Regestration" page
        When customer tries to create an account with inappropriate password
        Then error message indicating that password does not match the requirements is displayed


    Scenario: Successful Login To Customer Account
        Given customer is on the login page
        When customer log in with his valid credentials
        Then page with user account is displayed


    Scenario: Login With Invalid Credentials
        Given customer is on the login page
        When customer tries to log in with invalid credentials
        Then message indicating that incorrect log in credentials was provided is displayed and page is not changed

