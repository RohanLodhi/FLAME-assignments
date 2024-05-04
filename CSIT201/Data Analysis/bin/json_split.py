import json

# Step 1: Load the original JSON data
with open('/home/rohan/Desktop/Flame Courses/CSIT201/Final Project/stats_match.json', 'r') as file:
    data = json.load(file)

# Assuming 'data' is a list of dictionaries and you want to split it into two equal parts
half = len(data) // 10
data_part1 = data[:half]
data_part2 = data[half:]

# Step 2: Write the first part to a new JSON file
with open('/home/rohan/Desktop/Flame Courses/CSIT201/Final Project/part1.json', 'w') as file:
    json.dump(data_part1, file, indent=4)

# Step 3: Write the second part to a new JSON file
with open('/home/rohan/Desktop/Flame Courses/CSIT201/Final Project/part2.json', 'w') as file:
    json.dump(data_part2, file, indent=4)
