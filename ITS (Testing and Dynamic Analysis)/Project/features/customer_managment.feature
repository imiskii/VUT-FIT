Feature: Customer Managment

    Scenario: Delete Customer Account
        Given administrator is loged in
        And administrator is on "Customers" page
        And Customers List is not empty
        When administrator marks certain customer in Customer List
        And clicks "Delete" button
        Then message indicating modification of Customer List is displayed
        And deleted customer is not presented in Customer List
    

    Scenario Outline: Edit Customer Data
        Given administrator is loged in
        And administrator is on "Edit Customer" page of certain customer
        When administrator changes <data> about customer
        And clicks "Save" button
        Then message indicating successful modification is displayed
        And data about this customer are changed

        Examples:
            | data              |
            | Last Name         |


    Scenario Outline: Search Customer
        Given administrator is loged in
        And administrator is on "Customers" page
        And Customer List is not empty
        When administrator sets customer filter <filter>
        And clicks "Filter" button
        Then customers that match the specified filter are displayed
        But customers that do not match the specified filter are not displayed

        Examples:
            | filter            |
            | Customer Name     |


