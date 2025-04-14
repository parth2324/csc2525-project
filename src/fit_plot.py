import matplotlib.pyplot as plt

def read_segments(filename):
    result = []
    with open(filename, 'r') as file:
        while True:
            try:
                arr_len = int(file.readline().strip())
                slope = float(file.readline().strip())
                arr = [0 for _ in range(arr_len)]
                for i in range(arr_len):
                    arr[i] = float(file.readline().strip())
                result.append((slope, arr))
            except Exception as e:
                # Break the loop if we encounter any error (likely EOF)
                break
    return result

def plot_segment_fit(segment, slope, error_lines=True):
    low = segment[0]

    y_vals_actl = list(range(len(segment)))
    y_vals_pred = [slope * (key - low) for key in segment]
    
    plt.scatter(segment, y_vals_actl, color='blue', label='Actual', zorder=5)
    plt.plot(segment, y_vals_pred, color='red', label=f'Prediction using m = {slope}', linestyle='--', zorder=1)

    if not error_lines:
        return

    for i in range(1, len(segment)):
        xs = [segment[i], segment[i]]
        ys = [y_vals_actl[i], y_vals_pred[i]]

        plt.plot(xs, ys, color='green', linestyle=':', label="")
        plt.text((int)(segment[i] * 1.02), (ys[0] + ys[1]) / 2, f'{abs(ys[0] - ys[1]):.2f}', color='black', fontsize=6)


collec = read_segments("seg_cone_out.txt")
print("Number of cone segements read:", len(collec))
for slp, seg in collec:
    plot_segment_fit(seg, slp, True)

plt.xlabel('Keys')
plt.ylabel('Location')
plt.title('Cone Fit Plot')
plt.savefig("cone_fit_plot.png")

plt.figure()
collec = read_segments("seg_dcone_out.txt")
print("Number of cone segements read:", len(collec))
for slp, seg in collec:
    plot_segment_fit(seg, slp, True)

plt.xlabel('Keys')
plt.ylabel('Location')
plt.title('Dcone Fit Plot')
plt.savefig("dcone_fit_plot.png")
