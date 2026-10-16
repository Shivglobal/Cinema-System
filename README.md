Design a cinema scheduling system operating from 10 AM to 11 PM.
Implement canSchedule(movie, schedule) without overlapping bookings.
Follow-ups: Replace movies and calculate revenue.


[ Client / Admin Dashboard ]
             │
             ▼
    [ API Gateway / Load Balancer ]
             │
             ├──────────────────────────┐
             ▼                          ▼
   [ Scheduling Service ]     [ Reporting / Revenue Service ]
   - Time-slot validation     - Revenue aggregation
   - Conflict detection       - Daily financial reports
   - Screening CRUD & Updates
             │
             ▼
    [ Relational Database (SQL) ]
    - Screens, Movies, Screenings Tables



     Core Microservices / Modules:API Gateway: Entry point for admin requests (add, update, replace, view schedule). Handles authentication and request routing.Scheduling Service: The core business logic engine. Enforces time boundaries (10 AM - 11 PM), validates intervals to prevent overlaps, and manages screen allocations.Reporting / Revenue Service: Computes cumulative and per-screening projected/actual revenues.Data Persistence Layer: A relational database (PostgreSQL/MySQL) is chosen over NoSQL to enforce strict ACID properties and unique constraints for time overlaps.3. Data Model & Schema Design (Relational Database)To support multiple screens and strict scheduling, a normalized relational schema is ideal.Table: screensColumnTypeDescriptionscreen_id (PK)INTUnique identifier for the auditorium/screennameVARCHARScreen name (e.g., "Screen 1 - IMAX")Table: moviesColumnTypeDescriptionmovie_id (PK)INTUnique movie identifiertitleVARCHARMovie titleduration_minsINTTotal runtime in minutesTable: screenings (Showtimes)ColumnTypeDescriptionscreening_id (PK)INTUnique booking identifierscreen_id (FK)INTWhich screen is playing the moviemovie_id (FK)INTMovie being screenedstart_timeINTMinutes from midnight (e.g., 600)end_timeINTCalculated start_time + durationprojected_revenueDECIMALExpected revenue for the slotNote: A database-level check constraint or exclusion constraint can be applied to ensure no two rows on the same screen_id have overlapping [start_time, end_time] intervals.4. Core Workflows & Business LogicA. The Scheduling Check (canSchedule)When an admin requests to add or replace a screening, the system executes validation checks:Boundary Check:$$\text{start\_time} \ge 600 \quad \text{and} \quad \text{end\_time} \le 1380$$Overlap Check (Interval Intersection):For any existing screening $S$ on the target screen, check if:$$\max(\text{start}_1, \text{start}_2) < \min(\text{end}_1, \text{end}_2)$$If true, an overlap exists, and the transaction is rejected.B. Replacement Workflow (replaceScreening)Fetch & Verify: Retrieve the existing screening record by ID.Temporary Exclusion: Run the canSchedule check while temporarily ignoring the current screening's existing time slot (to allow shifting a movie slightly or changing its runtime without colliding with itself).Atomic Update: Update the record inside a database transaction (BEGIN TRANSACTION ... COMMIT).5. Concurrency & Scalability ConsiderationsRace Conditions (Double Booking): If two admins attempt to book the exact same screen slot simultaneously, a standard read-check-write pattern can cause a race condition.Solution: Use Database Transactions with Pessimistic Locking (SELECT ... FOR UPDATE) or Serializable Isolation Levels on the screenings table for the target screen ID during write operations.Caching: Read-heavy operations (like public schedule browsing) can be cached in Redis with a short TTL, reducing load on the primary SQL database.
