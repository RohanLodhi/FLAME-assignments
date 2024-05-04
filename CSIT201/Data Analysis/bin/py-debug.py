import json
# Load and display basic information about the JSON file to understand its top-level structure
with open("./CSIT201/Final Project/part1.json", 'r') as file:
    json_data = json.load(file)

# Output the type of the JSON root
print("Root type:", type(json_data))

# Depending on the type, print further information
if isinstance(json_data, dict):
    print("Root keys:", json_data.keys())  # Show all top-level keys
    # Try to print a snippet from each key if it's safe (not too large)
    for key, value in json_data.items():
        print(f"Key: {key}, Type: {type(value)}, Snippet: {str(value)[:100]}")  # Show first 100 chars
elif isinstance(json_data, list):
    print("Length of list:", len(json_data))
    if json_data:
        print("Type of first item:", type(json_data[0]))
        print("First item:", json_data[0])
else:
    print("Data content:", json_data)  # If it's neither a list nor a dict, just show it directly
