mapfile -t sources < <(find . -path "./build" -prune -o -type f \( -name "*.cpp" -o -name "*.c" -o -name "*.cc" \) -print | sort -u)

mapfile -t include_dirs < <(find . -path "./build" -prune -o -type f \( -name "*.h" -o -name "*.hh" -o -name "*.hpp" \) -exec dirname {} + | sort -u)

includes=""
for element in "${include_dirs[@]}"; do
    includes+=" -I ${element}"
done

echo ${sources[@]} ${includes}

