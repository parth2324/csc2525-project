import pandas as pd
import matplotlib.pyplot as plt
import argparse

def plot_distribution(file_name, column_name):
    # Load data
    df = pd.read_csv(file_name)

    # Check if column exists
    if column_name not in df.columns:
        print(f"Error: Column '{column_name}' not found in {file_name}")
        print(f"Available columns: {list(df.columns)}")
        return

    # Drop NaN and plot
    data = df[column_name].dropna()

    plt.figure(figsize=(8, 5))
    plt.hist(data, bins=30, edgecolor='black', alpha=0.7)
    plt.title(f'Distribution of {column_name}')
    plt.xlabel(column_name)
    plt.ylabel('Frequency')
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot a distribution from a CSV file.")
    parser.add_argument("file", help="Path to the CSV file")
    parser.add_argument("column", help="Name of the column to visualize")
    args = parser.parse_args()

    plot_distribution(args.file, args.column)