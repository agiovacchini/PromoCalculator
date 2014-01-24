Feature: Test kStore Servlet

  Scenario: Simple shopping on new cart

    Given request to initialize a new session

    Given request to open a new cart
    Then salesServer should return RC 0 and a cart id

    #Scenario: I use existing cart "1"

    Given request to add "1" quantity of item "260177500349" to cart
    Then salesServer should return RC 0 and my cart id

    Given request to add "1" quantity of item "8033604138826" to cart
    Then salesServer should return RC 0 and my cart id

    Given request to add "1" quantity of item "80642862" to cart
    Then salesServer should return RC 0 and my cart id

    Given request to add "1" quantity of item "2071775003426" to cart
    Then salesServer should return RC 0 and my cart id

    Given request to add "5" quantity of item "8710163214634" to cart
    Then salesServer should return RC 0 and my cart id

    Given request to remove item "8710163214634" from cart
    Then salesServer should return RC 0 and my cart id

    #Item not in cart
    Given request to remove item "163214634" from cart
    Then salesServer should return RC 1 and my cart id

    Given request to remove item "8033604138826" from cart
    Then salesServer should return RC 0 and my cart id

    Given request to remove item "8033604138826" from cart
    Then salesServer should return RC 1 and my cart id

    Given request to add "1" quantity of item "8033604138826" to cart
    Then salesServer should return RC 0 and my cart id

    Given request to add "2" quantity of item "8006545105561" to cart
    Then salesServer should return RC 0 and my cart id

    Given request to print cart
    Then salesServer should return RC 0 and my cart id

    Given request to persist cart
    Then salesServer should return RC 0 and my cart id

    #Given I wait forever

    Given request to close connection

  Scenario: Simple shopping on existing cart

    Given request to initialize a new session

    Given I use existing cart "1"

    Given request to add "1" quantity of item "8033604138826" to cart
    Then salesServer should return RC 0 and my cart id

    Given request to add "5" quantity of item "8710163214634" to cart
    Then salesServer should return RC 0 and my cart id

    Given request to remove item "8710163214634" from cart
    Then salesServer should return RC 0 and my cart id

    Given request to add "9999" quantity of item "8003653032703" to cart
    Then salesServer should return RC 0 and my cart id

    Given request to print cart
    Then salesServer should return RC 0 and my cart id

    Given request to persist cart
    Then salesServer should return RC 0 and my cart id

  #Given I wait forever

    Given request to close connection
    Then salesServer should return RC 0 and my cart id
