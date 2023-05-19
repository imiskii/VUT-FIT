Feature: Product Managment


    Scenario: Add New Product
        Given administrator is loged in
        And administrator is on Add Product page
        When administrator fills all necessary data for new product
        And clicks "Save" product button
        Then message indicating successful product modification is displayed


    Scenario Outline: Search Product
        Given administrator is loged in
        And administrator is on "Products" page
        And Product List is not empty
        When administrator sets product filter <filter>
        And click "Filter" button
        Then products that match the specified filter are displayed
        But products that do not match the specified filter are not displayed

        Examples:
            | filter        |
            | Product Name  |


    Scenario: Edit Product
        Given administrator is loged in
        And administrator is on Edit Product page of certain product
        When administrator changes data about product
        And click "Save" changes button
        Then changed product data are applied


    Scenario: Delete Product
        Given administrator is loged in
        And administrator is on "Products" page
        And Product List is not empty
        When administrator marks product in Product List
        And click "Delete" button
        Then message indicating successful Product modification is displayed
