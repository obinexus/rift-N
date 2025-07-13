# zettl_extractor.py
import os
import re
import json
from collections import defaultdict
import networkx as nx
import matplotlib.pyplot as plt

# === CONFIG ===
BASE_PATH = "./"  # Set to your Zettelkasten root
TARGET_FOLDERS = [
    "RIFTLang-KnowledgeBase",
    "GosiLang-KnowledgeBase",
    "NLink-KnowledgeBase",
    "LibRIFT-KnowledgeBase",
    "RIFTTools-KnowledgeBase"
]
NOTE_EXT = ".md"

# === DATA STRUCTURES ===
zettels = []
tags_graph = defaultdict(set)

# === UTILS ===
def extract_tags(content):
    return re.findall(r"#(\w+)", content)

def classify_note(filename, content, path):
    # By filename or tag
    if "insight" in filename:
        return "insight"
    elif "final" in filename:
        return "final"
    elif "temp" in filename:
        return "temp"
    tags = extract_tags(content)
    for t in tags:
        if t in ["insight", "final", "temp"]:
            return t
    return "unclassified"

def infer_context_tags(path):
    path = path.lower()
    tags = set()
    if "research" in path:
        tags.add("research")
    if "development" in path:
        tags.add("development")
    return list(tags)

# === SCAN FILES ===
for folder in TARGET_FOLDERS:
    base_dir = os.path.join(BASE_PATH, folder)
    for root, dirs, files in os.walk(base_dir):
        for file in files:
            if file.endswith(NOTE_EXT):
                filepath = os.path.join(root, file)
                with open(filepath, "r", encoding="utf-8") as f:
                    content = f.read()
                tags = extract_tags(content)
                extra_tags = infer_context_tags(filepath)
                tags.extend(t for t in extra_tags if t not in tags)
                note_type = classify_note(file.lower(), content.lower(), filepath)
                zettels.append({
                    "path": filepath,
                    "type": note_type,
                    "tags": tags,
                    "title": file.replace(".md", "")
                })
                for t in tags:
                    for other in tags:
                        if t != other:
                            tags_graph[t].add(other)

# === OUTPUT JSON INDEX ===
with open("zettlr_index.json", "w", encoding="utf-8") as f:
    json.dump(zettels, f, indent=2)

# === GRAPH PLOT ===
G = nx.Graph()
for tag, connections in tags_graph.items():
    for conn in connections:
        G.add_edge(tag, conn)

plt.figure(figsize=(12, 10))
pos = nx.spring_layout(G, k=0.4)
nx.draw(G, pos, with_labels=True, node_color='skyblue', node_size=1200, edge_color='gray')
plt.title("Zettelkasten Tag Graph")
plt.savefig("zettlr_tag_graph.png")
plt.show()
