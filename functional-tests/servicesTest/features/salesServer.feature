Feature: Test kStore Servlet

  Scenario: Simple shopping on new cart

    Given request to initialize a new session

    Given request to open a new cart

    Then salesServer should return RC "0" and a cart id

    #Scenario: I use existing cart "1"

    Given request to add "1" quantity of item "8033604138826" to cart

    Given request to add "5" quantity of item "8710163214634" to cart

    Given request to remove item "8710163214634" from cart

    Given request to print cart

    Given request to persist cart

    Given I wait forever

    #Given request to close connection

  Scenario: Simple shopping on existing cart

    Given request to initialize a new session

    Given I use existing cart "1"

    Given request to add "1" quantity of item "8033604138826" to cart

    Given request to add "5" quantity of item "8710163214634" to cart

    Given request to remove item "8710163214634" from cart

    Given request to print cart

    Given request to persist cart

    Given I wait forever
