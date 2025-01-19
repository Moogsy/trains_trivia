#!/bin/bash

# Define the database name
DB_NAME="trains.db"

# Define the ordered list of SQL files
SQL_FILES=(
    "schema.sql"
    "companies.sql"
    "lines.sql"
    "stations.sql"
    "station_lines/yamanote.sql"
    "station_lines/chuo-sobu.sql"
)

# Create or clear the database
if [ -f "$DB_NAME" ]; then
  echo "Database '$DB_NAME' already exists. Overwriting..."
  rm "$DB_NAME"
fi
echo "Creating database '$DB_NAME'."

# Execute each SQL file in order
for SQL_FILE in "${SQL_FILES[@]}"; do
  if [ ! -f "$SQL_FILE" ]; then
    echo "Error: SQL file '$SQL_FILE' not found."
    exit 1
  fi

  echo "Executing '$SQL_FILE'..."
  sqlite3 "$DB_NAME" < "$SQL_FILE"

  if [ $? -ne 0 ]; then
    echo "Error: Failed to execute '$SQL_FILE'."
    exit 1
  fi
done

echo "Database '$DB_NAME' created and all SQL files executed successfully."
