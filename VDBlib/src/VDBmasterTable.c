/******************************************************************************

GHAAS Command Line Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDBvariableDB.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <string.h>
#include <vdb.h>

static VDBmasterTable_t _VDBmasterTable [] = {
        {"biochemistry",  "c_litterfall",                      "C-LitterFall",                    "C LitterFall",                      "continuous", "grey",        "sum"},
        {"biochemistry",  "din_areal_loading",                 "DIN-AreaLoading",                 "DIN Areal Loading",                 "continuous", "grey",        "sum"},
        {"biochemistry",  "din_point_loading",                 "DIN-PointLoading",                "DIN Point Loading",                 "continuous", "grey",        "sum"},
        {"biochemistry",  "doc_areal_loading",                 "DOC-ArealLoading",                "DOC Areal Loading",                 "continuous", "grey",        "sum"},
        {"biochemistry",  "doc_point_loading",                 "DOC-PointLoading",                "DOC Point Loading",                 "continuous", "grey",        "sum"},
        {"biochemistry",  "don_areal_loading",                 "DON-ArealLoading",                "DON Areal Loading",                 "continuous", "grey",        "sum"},
        {"biochemistry",  "don_point_loading",                 "DON-PointLoading",                "DON Point Loading",                 "continuous", "grey",        "sum"},
        {"biochemistry",  "n_litterfall",                      "N-LitterFall",                    "N LitterFall",                      "continuous", "grey",        "sum"},
        {"biochemistry",  "nh4_areal_loading",                 "NH4-ArealLoading",                "NH4 Areal Loading",                 "continuous", "grey",        "sum"},
        {"biochemistry",  "nh4_point_loading",                 "Nh4-PointLoading",                "NH4 Point Loading",                 "continuous", "grey",        "sum"},
        {"biochemistry",  "no3_areal_loading",                 "NO3-ArealLoading",                "NO3 Areal Loading",                 "continuous", "grey",        "sum"},
        {"biochemistry",  "no3_point_loading",                 "NO3-PointLoading",                "NO3 Point Loading",                 "continuous", "grey",        "sum"},
        {"crop",          "growing_season1",                   "Crops-GrowingSeason1",            "Growing Season 1",                  "continuous", "grey",        "avg"},
        {"crop",          "growing_season2",                   "Crops-GrowingSeason2",            "Growing Season 2",                  "continuous", "grey",        "avg"},
        {"crop",          "irrigated_area_fraction",           "Irrigation-AreaFraction",         "Irrigated Area Fraction",           "continuous", "grey",        "avg"},
        {"crop",          "irrigation_efficiency",             "Irrigation-Efficiency",           "Irrigation Efficiency",             "continuous", "grey",        "avg"},
        {"crop",          "irrigation_gross_demand",           "IrrGrossDemand",                  "Irrigation Gross Demand",           "continuous", "grey",        "sum"},
        {"crop",          "cropland_fraction",                 "Cropland-Fraction",               "Cropland Fraction",                 "continuous", "grey",        "avg"},
        {"crop",          "cropping_intensity",                "Crops-Intensity",                 "Cropping Intensity",                "continuous", "grey",        "avg"},
        {"crop",          "other_crop_fraction",               "CropFraction-Other",              "Other Crop Fraction",               "continuous", "grey",        "avg"},
        {"crop",          "perennial_crop_fraction",           "CropFraction-Perennial",          "Perennial Crop Fraction",           "continuous", "grey",        "avg"},
        {"crop",          "rice_crop_fraction",                "CropFraction-Rice",               "Rice Crop Fraction",                "continuous", "grey",        "avg"},
        {"crop",          "rice_percolation_rate",             "Crops-RicePercolationRate",       "Rice Percolation Rate",             "continuous", "grey",        "avg"},
        {"crop",          "vegetables_crop_fraction",          "CropFraction-Vegetables",         "Vegetables Crop Fraction",          "continuous", "grey",        "avg"},
        {"energy",        "longwave_clear_sky_radiation",      "Radiation-LongWave-ClearSky",     "Clear Sky Long Wave Radiation",     "continuous", "grey",        "avg"},
        {"energy",        "longwave_downwelling_radiation",    "Radiation-LongWave-Downwelling",  "Downwelling Long Wave Radiation",   "continuous", "grey",        "avg"},
        {"energy",        "longwave_upwelling_radiation",      "Radiation-LongWave-Upwelling",    "Upwelling Long Wave Radiation",     "continuous", "grey",        "avg"},
        {"energy",        "shortwave_clear_sky_radiation",     "Radiation-ShortWave-ClearSky",    "Clear Sky Short Wave Radiation",    "continuous", "grey",        "avg"},
        {"energy",        "shortwave_downwelling_radiation",   "Radiation-ShortWave-Downwelling", "Downwelling Short Wave Radiation",  "continuous", "grey",        "avg"},
        {"energy",        "shortwave_upwelling_radiation",     "Radiation-ShortWave-Upwelling",   "Upwelling Short Wave Radiation",    "continuous", "grey",        "avg"},
        {"hydrology",     "daily_precipitation_fraction",      "Precipitation-DailyFraction",     "Daily Precipitation Fraction",      "continuous", "grey",        "avg"},
        {"hydrology",     "discharge",                         "Discharge",                       "Discharge",                         "continuous", "blue",        "avg"},
        {"hydrology",     "evapotranspiration",                "Evapotranspiration",              "Evapotranspiration",                "continuous", "grey",        "sum"},
        {"hydrology",     "potential_evapotranspiration",      "PotEvapotranspiration",           "Potential Evapotranspiration",      "continuous", "grey",        "sum"},
        {"hydrology",     "precipitation",                     "Precipitation",                   "Precipitation",                     "continuous", "blue",        "sum"},
        {"hydrology",     "rainfall",                          "Rainfall",                        "Rainfall",                          "continuous", "blue",        "sum"},
        {"hydrology",     "rain_pet",                          "RainPET",                         "Rain Potential ET",                 "continuous", "blue",        "sum"},
        {"hydrology",     "runoff",                            "Runoff",                          "Runoff",                            "continuous", "blue",        "avg"},
        {"hydrology",     "snowfall",                          "SnowFall",                        "Snowfall",                          "continuous", "blue",        "sum"},
        {"hydrology",     "soil_moisture",                     "SoilMoisture",                    "Soil Moisture",                     "continuous", "grey",        "avg"},
        {"hydrology",     "soil_moisture_change",              "SoilMoistureChange",              "Soil Moisture Change",              "continuous", "grey",        "avg"},
        {"land_cover",    "barren_percentage",                 "Landcover-PctBarren",             "Barren Percentage",                 "continuous", "grey",        "avg"},
        {"land_cover",    "deciduous_broadleaf_percentage",    "Landcover-PctBroadleafDeciduous", "Deciduous Broadleaf Percentage",    "continuous", "grey",        "avg"},
        {"land_cover",    "evergreen_broadleaf_percentage",    "Landcover-PctBroadleafEvergreen", "Evergreen Broadleaf Percentage",    "continuous", "grey",        "avg"},
        {"land_cover",    "deciduous_conifer_percentage",      "Landcover-PctConiferDeciduous",   "Deciduous Conifer Percentage",      "continuous", "grey",        "avg"},
        {"land_cover",    "evergreen_conifer_percentage",      "Landcover-PctConiferEvergreen",   "Evergreen Conifer Percentage",      "continuous", "grey",        "avg"},
        {"land_cover",    "cropland_percentage",               "Landcover-PctCropland",           "Cropland Percentage",               "continuous", "grey",        "avg"},
        {"land_cover",    "crop-natural_mosaic_percentage",    "Landcover-PctCropNaturalMosaic",  "Cropland-Natural Mosaic Percentage","continuous", "grey",        "avg"},
        {"land_cover",    "grassland_percentage",              "Landcover-PctGrassland",          "Grassland Percentage",              "continuous", "grey",        "avg"},
        {"land_cover",    "mixed_forest_percentage",           "Landcover-PctMixedForest",        "Mixed Forest Percentage",           "continuous", "grey",        "avg"},
        {"land_cover",    "mixed_savannah_percentage",         "Landcover-PctSavannah",           "Savannah Percentage",               "continuous", "grey",        "avg"},
        {"land_cover",    "mixed_woody_savannah_percentage",   "Landcover-PctSavannahWoody",      "Woody Savannah Percentage",         "continuous", "grey",        "avg"},
        {"land_cover",    "mixed_closed_shrubland_percentage", "Landcover-PctShrublandClosed",    "Closed Shrubland Percentage",       "continuous", "grey",        "avg"},
        {"land_cover",    "mixed_open_shrubland_percentage",   "Landcover-PctShrublandOpen",      "Open Shrubland Percentage",         "continuous", "grey",        "avg"},
        {"land_cover",    "mixed_snow_ice_percentage",         "Landcover-PctSnowIce",            "Snow-Ice Percentage",               "continuous", "grey",        "avg"},
        {"land_cover",    "urban_built-up_percentage",         "Landcover-PctUrbanBuiltUp",       "Urban Built-Up Percentage",         "continuous", "grey",        "avg"},
        {"land_cover",    "vegetation_percentage",             "Landcover-PctVegetation",         "Vegetation Fraction",               "continuous", "grey",        "avg"},
        {"land_cover",    "water_percentage",                  "Landcover-PctWater",              "Water Percentage",                  "continuous", "grey",        "avg"},
        {"land_cover",    "wetland_percentage",                "Landcover-PctWetland",            "Wetland Percentage",                "continuous", "grey",        "avg"},
        {"meteorology",   "air_temperature",                   "AirTemperature",                  "Air Temperature",                   "continuous", "blue-to-red", "avg"},
        {"meteorology",   "min_air_temperature",               "AirTemperature-Min",              "Minimum Air Temperature",           "continuous", "blue-to-red", "avg"},
        {"meteorology",   "max_air_temperature",               "AirTemperature-Max",              "Maximum Air Temperature",           "continuous", "blue-to-red", "avg"},
        {"meteorology",   "diurnal_air_temperature_range",     "AirTemperature-Range",            "Diurnal AirTemperature Range",      "continuous", "grey",        "avg"},
        {"meteorology",   "cloud_cover",                       "CloudCover",                      "Cloud Cover",                       "continuous", "grey",        "avg"},
        {"meteorology",   "heatindex",                         "Heatindex",                       "Heatindex",                         "continuous", "blue-to-red", "avg"},
        {"meteorology",   "max_heatindex",                     "Heatindex-Max",                   "Maximum Heatindex",                 "continuous", "blue-to-red", "avg"},
        {"meteorology",   "min_heatindex",                     "Heatindex-Min",                   "Minimum Heatindex",                 "continuous", "blue-to-red", "avg"},
        {"meteorology",   "dewpoint_temperature",              "Humidity-Dewpoint",               "Dewpoint Temperature",              "continuous", "bllue-to-red","avg"},
        {"meteorology",   "relative_humidity",                 "Humidity-Relative",               "Relative Humidity",                 "continuous", "blue",        "avg"},
        {"meteorology",   "specific_humidity",                 "Humidity-Specific",               "Specific Humidity",                 "continuous", "blue",        "avg"},
        {"meteorology",   "vapor_pressure",                    "Humidity-VaporPressure",          "Vapor Pressure",                    "continuous", "blue",        "avg"},
        {"meteorology",   "surface_air_pressure",              "AirPressure",                     "Air Pressure",                      "continuous", "grey",        "avg"},
        {"meteorology",   "v_wind_speed",                      "WindSpeed-V10m",                  "Wind Speed V-direction",            "continuous", "grey",        "avg"},
        {"meteorology",   "wind_speed",                        "WindSpeed",                       "Wind Speed",                        "continuous", "grey",        "avg"},
        {"meteorology",   "u_wind_speed",                      "WindSpeed-U10m",                  "Wind Speed U-direction",            "continuous", "grey",        "avg"},
        {"network",       "basin",                             "Basin",                           "Basin",                             "discrete",   "N/A",         "N/A"},
        {"network",       "mouth",                             "Mouth",                           "Mouth",                             "point",      "N/A",         "N/A"},
        {"network",       "cell_area",                         "CellArea",                        "Cell Area",                         "continuous", "grey",        "max"},
        {"network",       "cell_slope",                        "CellSlope",                       "Cell Slope",                        "continuous", "grey",        "avg"},
        {"network",       "confluence",                        "Confluence",                      "Confluence",                        "point",      "N/A",         "N/A"},
        {"network",       "distance_to_ocean",                 "DistToOcean",                     "Distance to Ocean",                 "continuous", "grey",        "max"},
        {"network",       "network",                           "Network",                         "STNetwork",                         "network",    "N/A",         "N/A"},
        {"network",       "ocean_basin",                       "OceanBasin",                      "Ocean Basin",                       "discrete",   "N/A",         "N/A"},
        {"network",       "streamline",                        "Streamline",                      "Streamline",                        "line",       "N/A",         "N/A"},
        {"network",       "subbasin",                          "Subbasin",                        "Subbasin",                          "discrete",   "N/A",         "N/A"},
        {"network",       "travel_time",                       "TravelTime",                      "Travel Time",                       "continuous", "grey",        "max"},
        {"network",       "upstream_area",                     "UpstreamArea",                    "Upstream Area",                     "continuous", "grey",        "avg"},
        {"reservoir",     "reservoir_capacity",                "Reservoir-Capacity",              "Reservoir Capacity",                "continuous", "grey",        "sum"},
        {"reservoir",     "small_reservoir_coefficient",       "SmallReservoir-Coefficient",      "Small-reservoir Coefficient",       "continuous", "grey",        "avg"},
        {"socio_economy", "population",                        "Population",                      "Population",                        "continuous", "grey",        "sum"},
        {"socio_economy", "population_density",	            "Population-Density",              "Population Density",               "continuous",  "grey",        "avg"},
        {"socio_economy", "population-rural",                  "Population-Rural",                "Rural Population",                  "continuous", "grey",         "avg"},
        {"socio_economy", "population-urban",                  "Population-Urban",                "Urban Population",                  "continuous", "grey",         "avg"},
        {"soil",          "field_capacity",                    "Soil-FieldCapacity",              "Field Capacity",                    "continuous", "grey",         "avg"},
        {"soil",          "rooting_depth",                     "Soil-RootingDepth",               "Rooting Depth",                     "continuous", "grey",         "sum"},
        {"soil",          "wilting_point",                     "Soil-WiltingPoint",               "Wilting Point",                     "continuous", "grey",         "avg"},
        {"spatial_unit",  "continents",                        "Continents",                      "Continents",                        "discrete",   "N/A",          "N/A"},
        {"spatial_unit",  "continent",                         "Continent",                       "Continent",                         "discrete",   "N/A",          "N/A"},
        {"spatial_unit",  "counties",                          "Counties",                        "Counties",                          "discrete",   "N/A",          "N/A"},
        {"spatial_unit",  "county",                            "County",                          "County",                            "discrete",   "N/A",          "N/A"},
        {"spatial_unit",  "countries",                         "Countries",                       "Countries",                         "discrete",   "N/A",          "N/A"},
        {"spatial_unit",  "country",                           "Country",                         "Country",                           "discrete",   "N/A",          "N/A"},
        {"spatial_unit",  "states",                            "States",                          "States",                            "discrete",   "N/A",          "N/A"},
        {"spatial_unit",  "state",                             "State",                           "State",                             "discrete",   "N/A",          "N/A"},
        {"spatial_unit",  "hydro_units",                       "Hydro-Units",                     "Hydrological Units",                "discrete",   "N/A",          "N/A"},
        {"spatial_unit",  "hydro_unit",                        "Hydro-Unit",                      "Hydrological Unit",                 "discrete",   "N/A",          "N/A"},
        {"station",       "discharge_gauge",                   "Discharge-Gauge",                 "Discharge Gauge",                   "point",      "N/A",          "N/A"},
        {"station",       "discharge_catchment",               "Discharge-Catchment",             "Discharge Catchment",               "discrete",   "N/A",          "N/A"},
        {"station",       "precipitation_gauge",               "Precipitation-Gauge",             "Precipitation Gauge",               "point",      "N/A",          "N/A"},
        {"station",       "power_plant",                       "PowerPlant",                      "Power Plant Catchment",             "point",      "N/A",          "N/A"},
        {"station",       "power_plant_catchment",             "PowerPlant-Catchment",            "Power Plant",                       "discrete",   "N/A",          "N/A"},
        {"station",       "reservoir",                         "Reservoir",                       "Reservoir",                         "point",      "N/A",          "N/A"},
        {"station",       "reservoir_catchment",               "Reservoir-Catchment",             "Reservoir Catchment",               "discrete",   "N/A",          "N/A"},
        {"topography",    "bathymetry",                        "bathymetry",                      "Bathymetry",                        "continuous", "blue",         "avg"},
        {"topography",    "bathymetry_min",                    "bathymetry-Min",                  "Minimum Depth",                     "continuous", "blue",         "avg"},
        {"topography",    "bathymetry_max",                    "bathymetry-Max",                  "Maximum Depth",                     "continuous", "blue",         "avg"},
        {"topography",    "elevation",                         "Elevation",                       "Elevation",                         "continuous", "elevation",    "avg"},
        {"topography",    "min_elevation",                     "Elevation-Min",                   "Minimum Elevation",                 "continuous", "elevation",    "avg"},
        {"topography",    "max_elevation",                     "Elevation-Max",                   "Maximum Elevation",                 "continuous", "elevation",    "avg"}};

#define VDBrecordNum sizeof( _VDBmasterTable)/sizeof(VDBmasterTable_t)

const char *VDBrgName (const char *cfName) {
    int i;

    for (i = 0; i < VDBrecordNum; ++i) {
        if (strcmp (cfName, _VDBmasterTable[i].CFname) == 0) return (_VDBmasterTable[i].GHAASname);
    }
    return ((const  char *) NULL);
}

const char *VDBlongName (const char *cfName) {
    int i;

    for (i = 0; i < VDBrecordNum; ++i) {
        if (strcmp (cfName, _VDBmasterTable[i].CFname) == 0) return (_VDBmasterTable[i].LongName);
    }
    return ((const  char *) NULL);
}

const char *VDBcfName (const char *rgName) {
    int i;

    for (i = 0; i < VDBrecordNum; ++i) {
        if (strcmp (rgName, _VDBmasterTable[i].GHAASname) == 0) return (_VDBmasterTable[i].CFname);
    }
    return ((const  char *) NULL);
}

const char *VDBdataType (const char *cfName) {
    int i;

    for (i = 0; i < VDBrecordNum; ++i) {
        if (strcmp (cfName, _VDBmasterTable[i].CFname) == 0) return (_VDBmasterTable[i].DataType);
    }
    return ((const  char *) NULL);
}

const char *VDBshadset (const char *cfName) {
    int i;

    for (i = 0; i < VDBrecordNum; ++i) {
        if (strcmp (cfName, _VDBmasterTable[i].CFname) == 0) return (_VDBmasterTable[i].Shadeset);
    }
    return ((const  char *) NULL);
}

const char *VDBaggregation (const char *cfName) {
    int i;

    for (i = 0; i < VDBrecordNum; ++i) {
        if (strcmp (cfName, _VDBmasterTable[i].CFname) == 0) return (_VDBmasterTable[i].Aggregation);
    }
    return ((const  char *) NULL);
}