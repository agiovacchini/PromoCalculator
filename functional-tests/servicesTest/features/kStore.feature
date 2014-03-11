Feature: Test kStore Servlet

  Scenario: For Benaglia
    Given request "A" to initialize a new session with device "12341"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026069320201"
    Then kStore should return RC "0" and the same card which has been sent

    #Coupon Dunnumby
    #Given existing request "A"
    #When I add kStore an item of type "SCAN_SDT_Code128" with code "6612053200000269161631401324239909"
    #Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8004817072962"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8013148027008"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN8" with code "80000532"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "2265210023807"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_UPCA" with code "701770117034"
    Then kStore should return RC "0"

    #Con vuoto
    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8001803200033"
    Then kStore should return RC "0"

    #Promo
    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8001943001989"
    Then kStore should return RC "0"

    Given existing request "A"
    When I suspend kStore transaction for POS "1"
    Then kStore should return RC "0"

    Given I wait forever

  Scenario: Running three shopping sessions
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026001142601"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8004817072962"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8013148027008"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN8" with code "80000532"
    Then kStore should return RC "0"



  #Initialize transaction B
    Given request "B" to initialize a new session with device "10002"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "2265210023807"
    Then kStore should return RC "0"

    Given request "C" to initialize a new session with device "10003"
    Then kStore should return RC "0" and a session id

  #Initialize transaction C
    Given existing request "C"
    When I add kStore a customer with code "026002893601"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_UPCA" with code "701770117034"
    Then kStore should return RC "0"

  #Con vuoto
    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8001803200033"
    Then kStore should return RC "0"

  #Promo
    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8001943001989"
    Then kStore should return RC "0"

   #Void transaction C
    Given existing request "C"
    When I void kStore transaction
    Then kStore should return RC "0"

    Given existing request "A"
    When I void kStore an item of type "SCAN_SDT_UPCA" with code "701770117034"
    Then kStore should return RC "0"

    Given existing request "A"
    When I suspend kStore transaction for POS "1"
    Then kStore should return RC "0"

  #Void transaction B
    Given existing request "B"
    When I void kStore transaction
    Then kStore should return RC "0"

  #Temoprarily not used because it kills the Toshiba Virtual Sales
  Scenario: I want to try adding an existing and not enabled card
    #Given request "B" to initialize a new session with device "12341"
    #Then kStore should return RC "0" and a session id

    #Given existing request "B"
    #When I add kStore a customer with code "026261997511"
    #Then kStore should return RC "3" and the same card which has been sent





  Scenario: Running one shopping session with promo item
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026007667001"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8001943001989"
    Then kStore should return RC "0"
    Then kStore should return promotional information with a discount amount of "1.2" and "SCONTO %" description

    Given existing request "A"
    When I suspend kStore transaction for POS "1"
    Then kStore should return RC "0"
    Then I close transaction on POS





  Scenario: Running one shopping session with promo item having two promotions
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026005784701"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8004817081179"
    Then kStore should return RC "0"
    Then kStore should return promotional information with a discount amount of "0.5" and "SCONTO %" description

    Given existing request "A"
    When I suspend kStore transaction for POS "1"
    Then kStore should return RC "0"
    Then I close transaction on POS

  Scenario: Running one shopping session with voided item
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026001132601"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_UPCA" with code "701770117034"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8004817072962"
    Then kStore should return RC "0"

    Given existing request "A"
    When I void kStore an item of type "SCAN_SDT_UPCA" with code "701770117034"
    Then kStore should return RC "0"

    Given existing request "A"
    When I suspend kStore transaction for POS "1"
    Then kStore should return RC "0"
    Then I close transaction on POS


  Scenario: Running one shopping sessions
    Given request "A" to initialize a new session with device "10001"
    Then kStore should return RC "0" and a session id

    Given existing request "A"
    When I add kStore a customer with code "026004995401"
    Then kStore should return RC "0" and the same card which has been sent

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8004817072962"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8013148027008"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN8" with code "80000532"
    Then kStore should return RC "0"

    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "2265210023807"
    Then kStore should return RC "0"


    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_UPCA" with code "701770117034"
    Then kStore should return RC "0"

  #Con vuoto
    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8001803200033"
    Then kStore should return RC "0"

  #Promo
    Given existing request "A"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8001943001989"
    Then kStore should return RC "0"

    Given existing request "A"
    When I void kStore an item of type "SCAN_SDT_UPCA" with code "701770117034"
    Then kStore should return RC "0"

    #Given existing request "A"
    #When I suspend kStore transaction for POS "1"
    #Then kStore should return RC "0"

  #Void transaction A
    #Given existing request "A"
   #When I void kStore transaction
    #Then kStore should return RC "0"

  Scenario: I want to try adding an existing and not enabled card
    Given request "C" to initialize a new session with device "12341"
    Then kStore should return RC "0" and a session id

    Given existing request "C"
    When I add kStore a customer with code "026005784701"
    Then kStore should return RC "3" and the same card which has been sent

  #When I ask for totals
    #Then kStore should return RC "0"
    #When I ask for totals
    #Then kStore should return RC "0"

    Given existing request "A"
    When I suspend kStore transaction for POS "2"
    Then kStore should return RC "0"
    #Then I close transaction on POS

  Scenario: I want to recover a previous session after disconencting and reconnecting
    Given existing session "18" with request id "Z" and last request number "4" and device "10001"
    When I recover session
    Then kStore should return RC "0"

    Given existing request "Z"
    When I add kStore an item of type "SCAN_SDT_EAN13" with code "8000754914679"
    Then kStore should return RC "0"

    Given existing request "Z"
    When I suspend kStore transaction for POS "1"
    Then kStore should return RC "0"
    Then I close transaction on POS
