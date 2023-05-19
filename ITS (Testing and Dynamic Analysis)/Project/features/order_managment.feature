Feature: Order Management

    Scenario: Delete Order
        Given administrator is loged in
        And administrator is on "Orders" page
        And Orders List is not empty
        When administrator marks certain order
        And clicks order "Delete" button
        Then message indicating modification of Orders List is displayed
        And deleted order is not presented in the Orders List


    Scenario Outline: Edit Order Status
        Given administrator is loged in
        And administrator is on certain order edit page
        When administrator changes order status to <status>
        And clicks the "Add History" button
        Then message indicating modification of order is displayed
        And selected status with current date is shown in History of statuses of current order

        Examples:
            | status    |
            | Shipped   |


    Scenario Outline: Search Order
        Given administrator is loged in
        And order list is not empty
        And administrator is on "Orders" page
        When administrator sets order filter <filter>
        And clicks order "Filter" button
        Then orders that match the specified filter are displayed
        But orders that do not match the specified filter are not displayed

        Examples:
            | filter        |
            | Order Status  |