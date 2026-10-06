#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "$0")" && pwd)"
figure_dir="$(cd "$script_dir/../figures" && pwd)"
config="$script_dir/mermaid-config.json"
puppeteer_config="$script_dir/puppeteer-config.json"

for source in "$script_dir"/*.mmd; do
    name="$(basename "$source" .mmd)"
    echo "Rendering $name..."
    # 寬版的 flowchart/sequence 圖改用不限制寬度、字體較大的設定，避免印出時文字過小。
    case "$name" in
        app_c_c4-component|app_c_sequence-line) diagram_config="$script_dir/mermaid-config-print.json" ;;
        *) diagram_config="$config" ;;
    esac
    mmdc -i "$source" -o "$figure_dir/$name.png" -c "$diagram_config" -p "$puppeteer_config" -b white -s 2
done
