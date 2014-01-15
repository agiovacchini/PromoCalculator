Feature: Test kStore Servlet

  Scenario: Simple shopping
    Given request to initialize a new session
    Then salesServer should return RC "0" and a cart id

    Given request to open a new cart

    Given request to add "1" quantity of item "8033604138826" to cart "1"

    Given request to add "5" quantity of item "8710163214634" to cart "1"

    Given request to remove item "8710163214634" from cart "1"

    Given request to print cart "1"

    Given request to persist cart "1"

    Given request to close connection
