// ItemCounter.mc
// Description: Class that handles counting between objects between 0 and a maximum count. 
//              Handles rollover if incrementing or decrementing falls above or below this.
// By: Ivan Chow
// Date: March 12, 2018

class ItemCounter {

    hidden var currentCount;
    hidden var maxCount;
    
    function initialize(refMaxCount) {
        currentCount = 0;
        maxCount = refMaxCount;
    }
    
    // Increments the current counter - handling rollover
    function increment() {
        currentCount++;
        
        if(currentCount > maxCount) {
            currentCount = 0;
        }
    }
    
    // Decrements the current counter - handling rollover
    function decrement() {
        currentCount--;
        
        if(currentCount < 0) {
            currentCount = maxCount;
        }
    }
    
    // Gets the current count
    function getCount() {
        return currentCount;
    }
}