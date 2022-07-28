#!/usr/bin/env python3.8
# coding=utf-8

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns
import numpy as np
import os
# muzete pridat libovolnou zakladni knihovnu ci knihovnu predstavenou na prednaskach
# dalsi knihovny pak na dotaz

# Ukol 1: nacteni dat
def get_dataframe(filename: str = "accidents.pkl.gz",
                  verbose: bool = False
                  ) -> pd.DataFrame:
    # read the dataframe
    df = pd.read_pickle(filename) # df = pd.read_pickle('accidents.pkl.gz')

    # count memory usage for all data columns in the dataframe before any changes
    if verbose:
        size_before = 0
        for usage in df.memory_usage(index=False, deep=True):
            size_before += usage

    # change type of selected columns to category
    columns = ['p36', 'weekday(p2a)', 'p6', 'p7', 'p8', 'p9', 'p10', 'p11', 'p12', 'p15', 'p16', 'p17', 'p18', 'p19', 'p20', 'p21', 'p22', 'p23', 'p24', 'p27', 'p28', 'p35', 'p39', 'p44', 'p45a', 'p47', 'p48a', 'p49', 'p50a', 'p50b', 'p51', 'p52', 'p55a', 'p57', 'p58', 'j', 'k', 'p', 'q', 't', 'p5a']
    # TODO - some letter named columns aren't worth the string type (integer would be better for 'n'...)
    # TODO - potentially use cut on 'p12', 'p35', 'p52'? it is category for now
    # TODO - 'p47' (production year of the car) is of category type too, which may or may not make sense
    # TODO - even some integer type columns could be categorized and it would make sense, e.g. number of cars present at the accident
    for col in columns:
        df[col] = df[col].astype('category')

    # make column 'date' of type datetime from column 'p2a'
    df['p2a'] = df['p2a'].astype('datetime64[D]')
    df = df.rename(columns={'p2a': 'date', 'weekday(p2a)': 'weekday(date)'})

    # count memory usage for all data columns in the dataframe after all the changes and print the before and after results
    if verbose:
        size_after = 0
        for usage in df.memory_usage(index=False, deep=True):
            size_after += usage
        print(f'orig_size={(size_before /  1_048_576):.1f} MB')
        print(f'new_size={(size_after /  1_048_576):.1f} MB')
    return df

# Ukol 2: následky nehod v jednotlivých regionech
def plot_conseq(df: pd.DataFrame, fig_location: str = None,
                show_figure: bool = False):
    if not fig_location and not show_figure:
        return

    # pick only needed columns from dataframe
    subdf = df[['region', 'p13a', 'p13b', 'p13c', 'p1']]
    # group data by region
    grouped = subdf.groupby('region')
    # aggregate the data
    aggregated = grouped.aggregate({'p13a': np.sum, 'p13b': np.sum, 'p13c': np.sum, 'p1': 'count'})
    # sort the rows
    aggregated = aggregated.sort_values('p1', ascending=False)
    # plot graphs
    axes = aggregated.plot.bar(subplots=True,
        title=['People died', 'People severely injured', 'People slightly injured', 'Total accidents'],
        legend=False,
        xlabel='Region',
        # ylabel='Count', # obvious from the graph titles
        figsize=(7, 10))
    for ax in axes:
        ax.set_facecolor('xkcd:ivory')
        ax.set_axisbelow(True)
        ax.yaxis.grid(color='gray', linestyle='dashed')
    if fig_location:
        if os.path.dirname(fig_location):
            os.makedirs(os.path.dirname(fig_location), exist_ok=True)
        plt.savefig(fig_location)
    if show_figure:
        plt.show()

# Ukol3: příčina nehody a škoda
def plot_damage(df: pd.DataFrame, fig_location: str = None,
                show_figure: bool = False):
    if not fig_location and not show_figure:
        return

    # pick only needed columns from dataframe
    subdf = pd.DataFrame()
    subdf['region'] = df['region']
    # categorize the numerical columns, change the value to multiples of 1000 rather than 100
    bins = pd.IntervalIndex.from_tuples([(100, 100), (201, 209), (301, 311), (401, 414), (501, 516), (601, 615)], closed='both')
    p12 = pd.cut(df['p12'].to_list(), bins)
    p12.categories = ['nezaviněná řidičem',
                      'nepřiměřená rychlost jízdy',
                      'nesprávné předjíždění',
                      'nedání přednosti v jízdě',
                      'nesprávný způsob jízdy',
                      'technická závada vozidla']
    subdf['p12'] = p12
    bins = pd.IntervalIndex.from_tuples([(0, 50), (50, 200), (200, 500), (500, 1000), (1000, float('inf'))], closed='left')
    p53 = pd.cut((df['p53'] / 10).to_list(), bins)
    p53.categories = ['< 50', '50 - 200', '200 - 500', '500 - 1000', '> 1000']
    subdf['p53'] = p53
    # TODO - PHA LBK ZLK KVK
    # tmp = subdf[subdf['region'] == 'PHA']
    # group data by region
    # aggregate the data
    # sort the rows
    # plot graphs
    ...
    # if fig_location:
    #     if os.path.dirname(fig_location):
    #         os.makedirs(os.path.dirname(fig_location), exist_ok=True)
    #     plt.savefig(fig_location)
    # if show_figure:
    #     plt.show()

# Ukol 4: povrch vozovky
def plot_surface(df: pd.DataFrame, fig_location: str = None,
                 show_figure: bool = False):
    if not fig_location and not show_figure:
        return

    ...
    if fig_location:
        if os.path.dirname(fig_location):
            os.makedirs(os.path.dirname(fig_location), exist_ok=True)
        plt.savefig(fig_location)
    if show_figure:
        plt.show()

if __name__ == "__main__":
    pass
    # zde je ukazka pouziti, tuto cast muzete modifikovat podle libosti
    # skript nebude pri testovani pousten primo, ale budou volany konkreni ¨
    # funkce.
    # df = get_dataframe("accidents.pkl.gz", True)
    df = get_dataframe("accidents.pkl.gz")
    # plot_conseq(df, fig_location="01_nasledky.png", show_figure=True)
    plot_conseq(df, fig_location="01_nasledky.png")
    # plot_damage(df, "02_priciny.png", True)
    plot_damage(df, "02_priciny.png")
    # plot_surface(df, "03_stav.png", True)
    plot_surface(df, "03_stav.png")

# TODO - add comments (PEP8 or whatever)
