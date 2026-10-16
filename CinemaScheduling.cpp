#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

struct Screening {
    int id;
    std::string title;
    int startTime; // minutes from midnight (e.g., 600 for 10:00 AM)
    int duration;  // in minutes
    double revenue; // expected revenue for this screening

    int getEndTime() const {
        return startTime + duration;
    }
};
class CinemaSchedule {
private:
    std::vector<Screening> schedule;
    const int OPEN_TIME = 10 * 60;  // 10:00 AM = 600 minutes
    const int CLOSE_TIME = 23 * 60; // 11:00 PM = 1380 minutes
    int nextId = 1;

    // Helper to check if two time intervals overlap
    bool isOverlapping(int start1, int end1, int start2, int end2) const {
        return std::max(start1, start2) < std::min(end1, end2);
    }

public:
    // Core function: Check if a movie can be scheduled without conflicts and within hours
    bool canSchedule(int startTime, int duration, int excludeId = -1) const {
        int endTime = startTime + duration;

        // 1. Check operating hours bounds (10 AM to 11 PM)
        if (startTime < OPEN_TIME || endTime > CLOSE_TIME) {
            return false;
        }

        // 2. Check for overlaps with existing bookings
        for (const auto& s : schedule) {
            if (s.id == excludeId) continue; // Skip itself if we are updating/replacing
            
            if (isOverlapping(startTime, endTime, s.startTime, s.getEndTime())) {
                return false; // Overlap detected!
            }
        }

        return true;
    }

    // Add a new movie screening
    bool addScreening(const std::string& title, int startTime, int duration, double revenue) {
        if (!canSchedule(startTime, duration)) {
            std::cout << "[Error] Cannot schedule '" << title << "': Time conflict or outside operating hours.\n";
            return false;
        }

        schedule.push_back({nextId++, title, startTime, duration, revenue});
        std::cout << "[Success] Scheduled '" << title << "' successfully.\n";
        return true;
    }

    // Follow-up 1: Replace/Update an existing movie screening by ID
    bool replaceScreening(int id, const std::string& newTitle, int newStartTime, int newDuration, double newRevenue) {
        auto it = std::find_if(schedule.begin(), schedule.end(), [id](const Screening& s) {
            return s.id == id;
        });

        if (it == schedule.end()) {
            std::cout << "[Error] Screening ID " << id << " not found for replacement.\n";
            return false;
        }

        // Validate using canSchedule while excluding the current ID
        if (!canSchedule(newStartTime, newDuration, id)) {
            std::cout << "[Error] Cannot replace screening: Time conflict or outside hours.\n";
            return false;
        }

        // Update booking details
        it->title = newTitle;
        it->startTime = newStartTime;
        it->duration = newDuration;
        it->revenue = newRevenue;

        std::cout << "[Success] Updated Screening ID " << id << " to '" << newTitle << "'.\n";
        return true;
    }

    // Follow-up 2: Calculate total revenue across all valid screenings
    double calculateTotalRevenue() const {
        double total = 0.0;
        for (const auto& s : schedule) {
            total += s.revenue;
        }
        return total;
    }

    // Display daily schedule
    void printSchedule() const {
        std::cout << "\n--- Daily Cinema Schedule (10:00 AM - 11:00 PM) ---\n";
        if (schedule.empty()) {
            std::cout << "No screenings scheduled.\n";
            return;
        }

        // Sort chronologically for clean display
        auto sortedSchedule = schedule;
        std::sort(sortedSchedule.begin(), sortedSchedule.end(), [](const Screening& a, const Screening& b) {
            return a.startTime < b.startTime;
        });

        for (const auto& s : sortedSchedule) {
            int startH = s.startTime / 60;
            int startM = s.startTime % 60;
            int endH = s.getEndTime() / 60;
            int endM = s.getEndTime() % 60;

            std::cout << "ID: " << s.id << " | "
                      << std::setfill('0') << std::setw(2) << startH << ":" << std::setw(2) << startM << " - "
                      << std::setw(2) << endH << ":" << std::setw(2) << endM 
                      << " | " << s.title << " ($" << std::fixed << std::setprecision(2) << s.revenue << ")\n";
        }
        std::cout << "---------------------------------------------------\n";
    }
};

int main() {
    CinemaSchedule cinema;

    // 1. Add valid screenings (Operating: 600 to 1380 minutes)
    // 10:00 AM (600) for 120 mins -> Ends at 12:00 PM
    cinema.addScreening("Inception", 600, 120, 1500.00); 

    // 12:30 PM (750) for 150 mins -> Ends at 3:00 PM
    cinema.addScreening("Avatar 2", 750, 150, 2200.50); 

    // Attempt an overlapping booking (Should Fail)
    // 11:00 AM (660) overlaps with Inception (600-720)
    cinema.addScreening("Interstellar", 660, 180, 1800.00); 

    // Attempt a booking outside operating hours (Should Fail)
    // 9:00 AM (540) is before opening time (600)
    cinema.addScreening("Early Bird Movie", 540, 90, 500.00);

    // Add another valid screening later in the day
    // 3:30 PM (930) for 120 mins -> Ends at 5:30 PM
    cinema.addScreening("The Matrix", 930, 120, 1200.00);

    cinema.printSchedule();

    // 2. Follow-up: Calculate initial total revenue
    std::cout << "Total Projected Revenue: $" << cinema.calculateTotalRevenue() << "\n\n";

    // 3. Follow-up: Replace/Reschedule a movie (ID 2: Avatar 2 -> Oppenheimer)
    // Changing slot to 4:00 PM (960) to 7:00 PM (1140)
    cinema.replaceScreening(2, "Oppenheimer", 960, 180, 2500.00);

    cinema.printSchedule();
    std::cout << "Updated Total Revenue: $" << cinema.calculateTotalRevenue() << "\n";

    return 0;
}
