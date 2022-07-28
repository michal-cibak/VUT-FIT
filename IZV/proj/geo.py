#!/usr/bin/python3.8
# coding=utf-8
import pandas as pd
import geopandas
import matplotlib.pyplot as plt
import contextily as ctx
import sklearn.cluster
import numpy as np
# muzeze pridat vlastni knihovny
import os


def make_geo(df: pd.DataFrame) -> geopandas.GeoDataFrame:
    """ Konvertovani dataframe do geopandas.GeoDataFrame se spravnym kodovani"""
    # change types of selected columns
    columns = ['p36', 'weekday(p2a)', 'p6', 'p7', 'p8', 'p9', 'p10', 'p11', 'p12', 'p15', 'p16', 'p17', 'p18', 'p19', 'p20', 'p21', 'p22', 'p23', 'p24', 'p27', 'p28', 'p35', 'p39', 'p44', 'p45a', 'p47', 'p48a', 'p49', 'p50a', 'p50b', 'p51', 'p52', 'p55a', 'p57', 'p58', 'j', 'k', 'p', 'q', 't', 'p5a']
    for col in columns:
        df[col] = df[col].astype('category')
    df['p2a'] = df['p2a'].astype('datetime64[D]')
    df = df.rename(columns={'p2a': 'date', 'weekday(p2a)': 'weekday(date)'})

    # convert dataframe to geodataframe, pick only rows with valid coordinates
    df_valid_loc = df[(df['d'] != np.nan) & (df['e'] != np.nan)]
    gdf = geopandas.GeoDataFrame(df_valid_loc, geometry=geopandas.points_from_xy(df['d'], df['e']), crs='EPSG:5514')
    return gdf


def plot_geo(gdf: geopandas.GeoDataFrame, fig_location: str = None,
             show_figure: bool = False):
    """ Vykresleni grafu s dvemi podgrafy podle lokality nehody """
    region = gdf[gdf['region'] == 'VYS']
    region_wm = region.to_crs('epsg:3857')

    # plot graph
    fig, ax = plt.subplots(1, 2, figsize=(10, 5))
    region_wm[region_wm['p5a'] == 1].plot(ax=ax[0], markersize=2, color='tab:blue')
    region_wm[region_wm['p5a'] == 2].plot(ax=ax[1], markersize=2, color='tab:red')
    ctx.add_basemap(ax=ax[0], crs=region_wm.crs.to_string(), source=ctx.providers.Stamen.TonerLite)
    ctx.add_basemap(ax=ax[1], crs=region_wm.crs.to_string(), source=ctx.providers.Stamen.TonerLite)
    fig.suptitle('Nehody v kraji: Kraj Vysočina - VYS', fontsize=16)
    ax[0].set_title('V obci')
    ax[1].set_title('Mimo obce')
    ax[0].axis('off')
    ax[1].axis('off')

    if fig_location:
        if os.path.dirname(fig_location):
            os.makedirs(os.path.dirname(fig_location), exist_ok=True)
        plt.savefig(fig_location) # plt.savefig('geo1.png')
    if show_figure:
        plt.show()


def plot_cluster(gdf: geopandas.GeoDataFrame, fig_location: str = None,
                 show_figure: bool = False):
    """ Vykresleni grafu s lokalitou vsech nehod v kraji shlukovanych do clusteru """
    region = gdf[gdf['region'] == 'VYS']
    region_wm = region.to_crs('epsg:3857')
    region_wm_noinf = region_wm[(region_wm['geometry'].x != float('inf')) & (region_wm['geometry'].y != float('inf'))]
    points_array = np.dstack([region_wm_noinf.geometry.x, region_wm_noinf.geometry.y]).reshape(-1, 2)
    clusters_array = sklearn.cluster.MiniBatchKMeans(n_clusters=50).fit(points_array)
    reg_wm_clst = region_wm_noinf.copy()
    reg_wm_clst['cluster'] = clusters_array.labels_
    reg_wm_clst = reg_wm_clst.dissolve(by='cluster', aggfunc={'p1': 'count'}).rename(columns={'p1': 'count'})
    reg_wm_clst['geometry'] = geopandas.points_from_xy(clusters_array.cluster_centers_[:, 0], clusters_array.cluster_centers_[:, 1])

    # plot graph
    fig, ax = plt.subplots(1, 1, figsize=(10, 5))
    reg_wm_clst.plot(ax=ax, markersize=(reg_wm_clst['count'] / 5), column='count', legend=True)
    region_wm_noinf.plot(ax=ax, markersize=1, color='tab:gray', alpha=0.1)
    ctx.add_basemap(ax=ax, crs=reg_wm_clst.crs.to_string(), source=ctx.providers.Stamen.TonerLite)
    ax.set_title('Nehody v kraji: Kraj Vysočina - VYS', fontsize=16)
    ax.axis('off')

    if fig_location:
        if os.path.dirname(fig_location):
            os.makedirs(os.path.dirname(fig_location), exist_ok=True)
        plt.savefig(fig_location) # plt.savefig('geo2.png')
    if show_figure:
        plt.show()


if __name__ == "__main__":
    # zde muzete delat libovolne modifikace
    gdf = make_geo(pd.read_pickle("accidents.pkl.gz"))
    plot_geo(gdf, "geo1.png", True)
    plot_cluster(gdf, "geo2.png", True)
