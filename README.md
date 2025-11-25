# Basic IoT Weather Station Data Logger
## Requirements

To make this work while you are working on this code you might need to install two things on your computer:
1. WSL on windows will allow you to install it. Follow steps for ubuntu or something it'll make it easier.

2. curl: `sudo apt install libcurl4-openssl-dev`
- for fetching data from the NWS api

3. nlohmann-json3-dev: `sudo apt install nlohmann-json3-dev`
- for us to parse the json files that are pullsed from the NWS (makes it usable)  

## Basic Outline
1. **Sensor Data Simulation:** Simulate real-time data collection from sensors measuring:
- Temperature
- Humidity
- Wind speed
- Atmospheric pressure
The data should be generated for different global locations to simulate diversity in environmental conditions.

2. **Data Collection and Logging:** Implement periodic data collection from all sensors and store the collected data in a log file. Each log entry must include:
- Timestamp
- Sensor type
- Sensor reading
- Geographic location

3. **Automated Responses:** Create automated responses that trigger actions when sensor data exceeds predefined thresholds (e.g., a warning when the temperature exceeds a specific value).

4. **Data Retrieval:** Implement a mechanism to retrieve historical data based on criteria such as:
- Date range
- Sensor type
- Location

5. **Data Processing:** Include basic data processing features, such as:

- Calculating daily averages
- Generating alerts when anomalies are detected
- Comparing data across different locations


## To-Do

- [x] Pull sensor data from API 
- [x] Organize data and store it
- [ ] Data retrieval from CSV based on criteria
- [ ] UML Diagram
- [ ] Data analysis (report averages, anomolies, compare data between locations, etc)
- [ ] Generate automated responses to data (warnings for high or low temperatures)

I am sure there is more to add to this list. Feel free to change or updated it!




## Deliverables

1. Source Code

2. Documentation
- Project overview
- UML class diagram
- Decription of modules/classes
- Usage instruction
- Sample outputs and screeshots
3. Presentation (5-10 minutes)
- Key features
- Design approach and UML diagram
- Challenges faced and solutions implemented

