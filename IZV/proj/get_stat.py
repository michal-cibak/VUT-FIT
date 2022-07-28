from download import DataDownloader
import argparse
import os
import numpy as np
import matplotlib.pyplot as plt

def plot_stat(data_source, fig_location = None, show_figure = False):
    """Plots a bar graph for each year showing number of accidents in each region.

    Positional arguments:
    data_source -- a tuple obtained from DataDownloader class in module download

    Keyword arguments:
    fig_location -- a path including filename where image of the graph should be saved
        (default None - image won't be saved)
    show_figure -- a boolean value stating whether the graph should open in an interactive window
        (default False - graph won't be shown)
    """
    if fig_location is None and show_figure == False:
        return

    years_regions, accidents = np.unique([data_source[1][3].astype('datetime64[Y]').astype('U'), data_source[1][-1]], axis=1, return_counts=True)
    years = np.unique(years_regions[0])
    regions = np.unique(years_regions[1])
    accidents = np.reshape(accidents, [len(years), len(regions)])

    fig, axes = plt.subplots(ncols=1, nrows=len(years), constrained_layout=True, figsize=(7, 10))
    fig.suptitle('Accidents in regions by year')
    for year, ax in enumerate(axes):
        bars = ax.bar(regions, accidents[year])
        unique_accidents, order = np.unique(accidents[year], return_inverse=True)
        for region, bar in enumerate(bars):
            ax.annotate(
                str(len(unique_accidents) - order[region]),
                xy=(bar.get_x() + bar.get_width() / 2, bar.get_height()),
                xytext=(0, 3),
                textcoords="offset points",
                ha='center',
                va='bottom')
        ax.set_xlabel('Region')
        ax.set_ylabel('Accidents')
        ax.set_title(years[year])
        ax.spines['top'].set_visible(False)
        ax.spines['right'].set_visible(False)

    if fig_location:
        if os.path.dirname(fig_location):
            os.makedirs(os.path.dirname(fig_location), exist_ok=True)
        plt.savefig(fig_location)

    if show_figure:
        plt.show()

    plt.close(fig)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--fig_location', help='path with filename where image of graph should be saved')
    parser.add_argument('--show_figure', help='show graph in a window', action='store_true')
    args = parser.parse_args()

    if args.fig_location or args.show_figure:
        data_source = DataDownloader().get_list(['PHA', 'STC', 'JHC'])
        plot_stat(data_source, fig_location=args.fig_location, show_figure=args.show_figure)
