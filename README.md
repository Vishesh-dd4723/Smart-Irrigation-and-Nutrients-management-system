# Smart Irrigation and nutrients management system

This system ease the task of taking day to day farmer's decision on the right time to irrigate the field and the right fertilizer to use based on the soil conditions.
The system takes into account the current moisture content, rain chances and optimum moisture content to decide when to turn the spriklers ON or OFF.
Also the nutrients in the soil are constantly monitored to suggest the farmer the type and amount of fertilizer to be used based on the planted crop.

## Equipments Used

- **Node MCU** : NodeMCU is used as a microcontroller for this project.
Its functions are:
   1. Comparing the optimal values of a particular crop's nutrients and moisture in soil.
   2. Notify the content of nutrients in soil and suggest the appropriate fertilizer.
   3. It also controls water by checking moisture content and weather's condition of next few days and ON/OFF sprinklers accordingly.
- **FC-28**: FC-28 is the soil moisture sensor.
- **NPK Sensor**: NPK Sensor is the sensor to measure the key nutrients in the soil: Nitrogen, Phosphorus and Pottasium.
- **LCD Display**: LCD Display module displays the rain chance within the upcoming 2-3 days, soil moisture content and nutrients content.
- **4X1 Analog MUX**: 4X1 MUX selects one of the four inputs - Soil Moisture, Nitrogen content, Phosphorus content and Pottasium content at a time and displays its value to the end user. 

## Workflow
The following flow chart describes the algorithm behind this system's working:-


![Flow-chart](https://github.com/Vishesh-dd4723/Productathon/blob/master/Screenshots/2021-03-14%20(2).png)
