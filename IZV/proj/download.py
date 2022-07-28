import numpy as np
import os
import requests
from bs4 import BeautifulSoup
import zipfile
import csv
import re
import gzip
import pickle

class DataDownloader:
    """A class containing methods for obtaining, parsing and storing specific statictical data.

    The statictical data are about road accidents which happened in Czech Republic.
    Methods in this class will ensure acquisition of the data from specified webpage
    (which should be in same format as the default one, see positional argument url),
    parsing of the data into a numpy representation and storing the parsed data
    in files as well as in an instance of this class.

    Positional arguments for instance creation:
    url -- a webpage where the data should be obtained from
        (default https://ehw.fit.vutbr.cz/izv/)
    folder -- a folder where data will be saved
        (default 'data')
    cache_filename -- a name including {} of file where parsed data are saved
        the name must include formatting string {} which stands for individual regions
        each region is stored in its respective file
        (default 'data_{}.pkl.gz')

    Public methods:
    download_data() -- downloads all archives with data
    parse_region_data() -- parses specified region into numpy representation
    get_list() -- gets parsed data for specified regions
    """

    region_filename = {'PHA': '00.csv', 'STC': '01.csv', 'JHC': '02.csv', 'PLK': '03.csv', 'ULK': '04.csv', 'KVK': '05.csv', 'JHM': '06.csv', 'MSK': '07.csv', 'OLK': '14.csv', 'ZLK': '15.csv', 'VYS': '16.csv', 'PAK': '17.csv', 'LBK': '18.csv', 'HKK': '19.csv'}
    data_header = ['p1', 'p36', 'p37', 'p2a', 'weekday(p2a)', 'p2b', 'p6', 'p7', 'p8', 'p9', 'p10', 'p11', 'p12', 'p13a', 'p13b', 'p13c', 'p14', 'p15', 'p16', 'p17', 'p18', 'p19', 'p20', 'p21', 'p22', 'p23', 'p24', 'p27', 'p28', 'p34', 'p35', 'p39', 'p44', 'p45a', 'p47', 'p48a', 'p49', 'p50a', 'p50b', 'p51', 'p52', 'p53', 'p55a', 'p57', 'p58', 'a', 'b', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'n', 'o', 'p', 'q', 'r', 's', 't', 'p5a', 'region']
    parsed_data = {'PHA': None, 'STC': None, 'JHC': None, 'PLK': None, 'KVK': None, 'ULK': None, 'LBK': None, 'HKK': None, 'PAK': None, 'OLK': None, 'MSK': None, 'JHM': None, 'ZLK': None, 'VYS': None}
    data_archive_names = list()

    def __init__(self, url="https://ehw.fit.vutbr.cz/izv/", folder="data", cache_filename="data_{}.pkl.gz"):
        self.url = url
        self.data_folder = folder
        self.cache = cache_filename

        if folder:
            os.makedirs(folder, exist_ok=True)

        self.get_data_archive_names()

    def download_data(self):
        """Downloads all archives with statistical data from the specified webpage.

        Webpage is the one specified in class positional argument 'url'.
        """
        headers = {
            'Connection': 'keep-alive',
            'Upgrade-Insecure-Requests': '1',
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36 OPR/67.0.3575.137',
            'Sec-Fetch-Dest': 'document',
            'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9',
            'Sec-Fetch-Site': 'none',
            'Sec-Fetch-Mode': 'navigate',
            'Accept-Language': 'sk-SK,sk;q=0.9,cs;q=0.8,en;q=0.7,en-US;q=0.6',
        }

        response = requests.get(self.url, headers=headers)

        content = BeautifulSoup(response.content, 'html.parser')
        for link in content.table.find_all('a'):
            print(f'Downloading file {link["href"].split("/")[-1]} from {self.url}{link["href"]} to folder {self.data_folder}') # DEBUG
            with requests.get(f'{self.url}{link["href"]}', stream=True) as data: # file download works without custom headers
                with open(os.path.join(self.data_folder, link["href"].split("/")[-1]), 'wb') as savefile:
                    for chunk in data.iter_content(chunk_size=16384):
                        savefile.write(chunk)

        self.get_data_archive_names()

    def download_missing_data(self):
        """Auxiliary function.

        Checks if there are any downloaded data in folder specified in class positional
        argument 'folder' and if there are not, downloads all using download_data() method.
        """
        if not self.data_archive_names:
            self.download_data()

    def get_data_archive_names(self):
        """Auxiliary function.

        Finds all zip archives in data folder, picks the latest one for each year
        and stores them in class variable 'data_archive_names'.
        """
        highest_in_year = dict()
        mm_yyyy = [None, None]
        for ford in os.listdir(self.data_folder):
            if os.path.isfile(os.path.join(self.data_folder, ford)) and re.findall('^datagis.*\.zip$', ford):
                mm_yyyy_cptobj = re.search('[0-9][0-9]-[0-9][0-9][0-9][0-9]', ford)
                if mm_yyyy_cptobj is None:
                    yyyy_cptobj = re.search('[0-9][0-9][0-9][0-9]', ford)
                    mm_yyyy[0] = '12'
                    mm_yyyy[1] = yyyy_cptobj.group()
                else:
                    mm_yyyy = mm_yyyy_cptobj.group().split('-')
                if mm_yyyy[1] in highest_in_year:
                    if mm_yyyy[0] > highest_in_year[mm_yyyy[1]][0]:
                        highest_in_year[mm_yyyy[1]] = (mm_yyyy[0], ford)
                else:
                    highest_in_year[mm_yyyy[1]] = (mm_yyyy[0], ford)

        for mm_filename in highest_in_year.values():
            # print(f'Last file for year is "{mm_filename[1]}"') # DEBUG
            self.data_archive_names.append(mm_filename[1])

    def parse_region_data(self, region):
        """Parses downloaded data for specified region and returns them.

        Data are taken from downloaded archives and are parsed into a tuple with
        data header and numpy representation of the data.

        Positional arguments:
        region -- abbreviation of region to be parsed

        Returns:
        A tuple consisting of 2 lists. First contains names of columns of data,
        second contains numpy lists with the data.
        """
        self.download_missing_data()

        data_arrays = list()
        for column in self.data_header:
            data_arrays.append(list())

        for data_archive in self.data_archive_names:
            with zipfile.ZipFile(os.path.join(self.data_folder, data_archive), 'r') as zf:
                # extract a file corresponding to the region as "tmp.scv"
                zipfileinfo = zf.getinfo(self.region_filename[region])
                zipfileinfo.filename = 'tmp.csv'
                zf.extract(zipfileinfo, path = self.data_folder)

            tmpfilepath = os.path.join(self.data_folder, 'tmp.csv')

            with open(tmpfilepath, newline='') as f:
                reader = csv.reader(f, delimiter=';', quotechar='"')
                for row in reader:
                    # convert strings to proper types
                    # integers
                    if row[34] == 'XX':
                        row[34] = -1
                    for index in (0, 1, 2, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 63):
                        if not row[index]:
                            row[index] = -1
                        row[index] = int(row[index])
                    # floats
                    for index in (45, 46, 47, 48, 49, 50):
                        row[index] = row[index].replace(',', '.')
                        if not row[index]:
                            row[index] = 0.0
                        try:
                            row[index] = float(row[index])
                        except ValueError:
                            row[index] = 0.0

                    # add values to their respective arrays
                    for column, item in enumerate(row):
                        data_arrays[column].append(item)
                    data_arrays[-1].append(region)

            # delete the temporary file corresponding to the region
            os.remove(tmpfilepath)

        # convert arrays with data to numpy arrays
        np_data_arrays = list()
        dates = data_arrays[3]
        data_arrays[3] = None # will be converted after the cycle to datetimes
        for data_array in data_arrays:
            np_data_arrays.append(np.array(data_array))
        np_data_arrays[3] = np.array(dates, dtype='M') # convert the dates to numpy datetimes

        return (self.data_header, np_data_arrays)

    def get_region_from_cache(self, path):
        """Auxiliary function."""
        with gzip.open(path,'rb') as f:
            np_data_arrays = pickle.load(f)
        return np_data_arrays

    def set_region_to_cache(self, path, np_data_arrays):
        """Auxiliary function."""
        with gzip.open(path,'wb') as f:
            pickle.dump(np_data_arrays, f)

    def get_list(self, regions=None):
        """Gets parsed data for specified regions and returns them.

        The parsed data either are already available in the class instance
        or in a cache file or are acquired using the parse_region_data() method.

        Positional arguments:
        regions -- list of abbreviations of regions whose data should be returned
            (default None - same as specifying all regions)

        Returns:
        A tuple consisting of 2 lists. First contains names of columns of data,
        second contains numpy lists with the data.
        """
        parsed_regions = list()

        if regions is None:
            regions = list()
            for region in self.parsed_data:
                regions.append(region)

        for region in regions:
            # load data for the region, add it into a list
            cache_path = os.path.join(self.data_folder, self.cache.format(region))
            if not self.parsed_data[region] is None: # parsed data are in memory
                # load parsed data from memory
                parsed_regions.append(self.parsed_data[region])
            elif os.path.isfile(cache_path): # parsed data are in cache
                # load parsed data from cache and store them in memory
                self.parsed_data[region] = self.get_region_from_cache(cache_path)
                parsed_regions.append(self.parsed_data[region])
            else: # data must be parsed
                # parse raw data, store result in cache and in memory
                region_data = self.parse_region_data(region)[1]
                self.set_region_to_cache(cache_path, region_data)
                self.parsed_data[region] = region_data
                parsed_regions.append(region_data)

        # concatenate all regions
        return (self.data_header, [np.concatenate(nth_column_from_each_region) for nth_column_from_each_region in zip(*parsed_regions)])

if __name__ == "__main__":
    downloader = DataDownloader()
    regions = ['PHA', 'STC', 'JHC'] # regions = list(downloader.parsed_data)[0:3]
    data = downloader.get_list(regions)
    print(f'Columns for region data:\n{", ".join(downloader.data_header)}')
    print('Selected region - Number of entries:')
    total = 0
    for region in regions:
        print(f'{region} - {downloader.parsed_data[region][0].size}')
        total += downloader.parsed_data[region][0].size
    print(f'Total entries for selected regions:\n{total}')
