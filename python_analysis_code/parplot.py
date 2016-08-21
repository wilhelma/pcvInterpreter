import json
import matplotlib.pyplot as plt

with open('parasite_output.json') as data_file:    
    data = json.load(data_file)

handles = []

for call_site in data["call_site_array"]:

	y = call_site["parallelism"],
	x1 = call_site["start"]
	x2 = call_site["stop"]
	handle = plt.plot((x1, x2), (y, y), linewidth=5, label=call_site["function_signature"])
	handles.append(call_site["function_signature"])

plt.ylim(0.8, 2.2)
plt.legend(handles)
plt.show()

# data["maps"][0]["id"] 
