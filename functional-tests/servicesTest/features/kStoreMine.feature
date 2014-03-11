Feature: Test kStore Servlet

  Scenario: Starting one simple shopping session
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026007249101"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8000754914679"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "2093940001013"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "2093940001013"
    Then kStore should return RC "0"

    Given existing request "A"
    When I suspend kStore transaction for POS "1"
    Then kStore should return RC "0"

  Scenario: Starting and closing a shopping session with item voids
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026007249201"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8000754914679"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8000754914679"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "2093940001013"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "2093940001013"
    Then kStore should return RC "0"

    Given existing request "A"
    When I void kStore an item of type "SCAN_SDT_EAN13" with code "8000754914679"
    Then kStore should return RC "0"

    Given existing request "A"
    When I void kStore an item of type "SCAN_SDT_EAN13" with code "8100754914679"
    Then kStore should return RC "3"

    Given existing request "A"
    When I suspend kStore transaction for POS "1"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN8" with code "80186649"
    Then kStore should return RC "0"

  Scenario: Starting one simple shopping session trying to add two times the same customer
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026007249101"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I add kStore a customer with code "026007249101"
    Then kStore should return RC "3" and the same card which has been sent

  Scenario: Starting one simple shopping session trying to add more than two customers
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026007249101"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore a customer with code "026007249201"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore a customer with code "026007249301"
    Then kStore should return RC "3"

    Given existing request "A"
    When I add kStore a customer with code "026007249401"
    Then kStore should return RC "3"

  Scenario: Starting one shopping with linked item
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026007249101"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8033286980218"
    Then kStore should return RC "0"

    Given existing request "A"
    When I suspend kStore transaction for POS "1"
    Then kStore should return RC "0"

  Scenario: Starting one shopping with linked item and its void
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026007249101"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8033286980218"
    Then kStore should return RC "0"

    Given existing request "A"
    When I void kStore an item of type "SCAN_SDT_EAN13" with code "8033286980218"
    Then kStore should return RC "0"

    Given existing request "A"
    When I suspend kStore transaction for POS "1"
    Then kStore should return RC "0"


  Scenario: Starting one shopping session, add item, void it and try to add item again
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026007249101"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8033286980218"
    Then kStore should return RC "0"

    Given existing request "A"
    When I void kStore transaction
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8100754914679"
    Then kStore should return RC "3"

    Given existing request "A"
    When I suspend kStore transaction for POS "1"
    Then kStore should return RC "3"

  Scenario: Starting one shopping session, add item, void it and add it again then add item and close transaction
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026007249101"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I void kStore a customer with code "026007249101"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I void kStore a customer with code "026007249101"
    Then kStore should return RC "3" and the same card which has been sent

    Given existing request "A"
    When I add kStore a customer with code "026007249101"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8033286980218"
    Then kStore should return RC "0"

    Given existing request "A"
    When I suspend kStore transaction for POS "1"
    Then kStore should return RC "0"
