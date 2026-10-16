#include <iostream>
#include <unordered_map>
#include <set>
#include <stdexcept>

class CommodityTracker {
private:
    std::unordered_map<int, double> timeToPrice;
    std::multiset<double> prices;

public:
    // Insert a new timestamp-price pair or update an existing timestamp
    void upsertCommodityPrice(int timestamp, double price) {
        // If the timestamp already exists, we are updating an existing price
        if (timeToPrice.find(timestamp) != timeToPrice.end()) {
            double oldPrice = timeToPrice[timestamp];
            
            // Remove ONLY ONE instance of the old price from the multiset
            auto it = prices.find(oldPrice);
            if (it != prices.end()) {
                prices.erase(it);
            }
        }
        
        // Insert the new price and update/add the mapping
        prices.insert(price);
        timeToPrice[timestamp] = price;
    }

    // Retrieve the maximum commodity price in O(1) time
    double getMaxCommodityPrice() const {
        if (prices.empty()) {
            throw std::runtime_error("No commodity prices available.");
        }
        return *prices.rbegin();
    }
};

// --- Example Usage ---
int main() {
    CommodityTracker tracker;

    // Inserting initial prices (out of order timestamps are naturally supported)
    tracker.upsertCommodityPrice(1, 10.0);
    tracker.upsertCommodityPrice(3, 25.0);
    tracker.upsertCommodityPrice(2, 15.0); // Out-of-order timestamp 2

    // Update an existing timestamp (Timestamp 2 price changes from 15.0 to 26.0)
    tracker.upsertCommodityPrice(2, 26.0);
    
    // Update another timestamp (Timestamp 1 price changes from 10.0 to 20.0)
    tracker.upsertCommodityPrice(1, 20.0);

    // Current prices in the tracker correspond to timestamps: 
    // T1=20, T2=26, T3=25
    std::cout << "Max Commodity Price: " << tracker.getMaxCommodityPrice() << "\n"; // Expected: 26.0

    return 0;
}
