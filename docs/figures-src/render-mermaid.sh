#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "$0")" && pwd)"
figure_dir="$(cd "$script_dir/../figures" && pwd)"
config="$script_dir/mermaid-config.json"
puppeteer_config="$script_dir/puppeteer-config.json"

for source in "$script_dir"/*.mmd; do
    name="$(basename "$source" .mmd)"
    echo "Rendering $name..."
    mmdc -i "$source" -o "$figure_dir/$name.png" -c "$config" -p "$puppeteer_config" -b white -s 2
done
