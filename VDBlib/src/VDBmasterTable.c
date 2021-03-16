/******************************************************************************

GHAAS Command Line Library V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

VDBvariableDB.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <string.h>
#include <vdb.h>

#define VDBcontinuous    "continuous"
#define VDBdiscrete      "discrete"
#define VDBline          "line"
#define VDBnetwork       "network"
#define VDBpoint         "point"

#define VDBblue          "blue"
#define VDBblue2red      "blue-to-red"
#define VDBelevation     "elevation"
#define VDBgrey          "grey"
#define VDBavg           "avg"
#define VDBmax           "max"
#define VDBsum           "sum"
#define VDBnotApplicable "N/A"

static VDBmasterTable_t _VDBmasterTable2 [] = {
        {"biochemistry",  "c_litterfall",                      "C-LitterFall",                    "C LitterFall",                      VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "din_areal_loading",                 "DIN-AreaLoading",                 "DIN Areal Loading",                 VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "din_point_loading",                 "DIN-PointLoading",                "DIN Point Loading",                 VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "doc_areal_loading",                 "DOC-ArealLoading",                "DOC Areal Loading",                 VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "doc_point_loading",                 "DOC-PointLoading",                "DOC Point Loading",                 VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "don_areal_loading",                 "DON-ArealLoading",                "DON Areal Loading",                 VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "don_point_loading",                 "DON-PointLoading",                "DON Point Loading",                 VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "n_litterfall",                      "N-LitterFall",                    "N LitterFall",                      VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "nh4_areal_loading",                 "NH4-ArealLoading",                "NH4 Areal Loading",                 VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "nh4_point_loading",                 "Nh4-PointLoading",                "NH4 Point Loading",                 VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "no3_areal_loading",                 "NO3-ArealLoading",                "NO3 Areal Loading",                 VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "no3_point_loading",                 "NO3-PointLoading",                "NO3 Point Loading",                 VDBcontinuous, VDBgrey,          VDBsum},
        {"crop",          "growing_season1",                   "Crops-GrowingSeason1",            "Growing Season 1",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "growing_season2",                   "Crops-GrowingSeason2",            "Growing Season 2",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "irrigated_area_fraction",           "Irrigation-AreaFraction",         "Irrigated Area Fraction",           VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "irrigation_efficiency",             "Irrigation-Efficiency",           "Irrigation Efficiency",             VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "irrigation_gross_demand",           "IrrGrossDemand",                  "Irrigation Gross Demand",           VDBcontinuous, VDBgrey,          VDBsum},
        {"crop",          "cropland_fraction",                 "Cropland-Fraction",               "Cropland Fraction",                 VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "cropping_intensity",                "Crops-Intensity",                 "Cropping Intensity",                VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "other_crop_fraction",               "CropFraction-Other",              "Other Crop Fraction",               VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "perennial_crop_fraction",           "CropFraction-Perennial",          "Perennial Crop Fraction",           VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "rice_crop_fraction",                "CropFraction-Rice",               "Rice Crop Fraction",                VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "rice_percolation_rate",             "Crops-RicePercolationRate",       "Rice Percolation Rate",             VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "vegetables_crop_fraction",          "CropFraction-Vegetables",         "Vegetables Crop Fraction",          VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_barley",              "CropFraction-Barley",             "Crop Fraction - Barley",            VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_cassava",             "CropFraction-Cassava",            "Crop Fraction - Cassava",           VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_cotton",              "CropFraction-Cotton",             "Crop Fraction - Cotton",            VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_groundnut",           "CropFraction-Groundnut",          "Crop Fraction - Groundnut",         VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_maize",               "CropFraction-Maize",              "Crop Fraction - Maize",             VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_millet",              "CropFraction-Millet",             "Crop Fraction - Millet",            VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_oilpalm",             "CropFraction-Oilpalm",            "Crop Fraction - Oilpalm",           VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_other",               "CropFraction-Other",              "Crop Fraction - Other",             VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_potato",              "CropFraction-Potato",             "Crop Fraction - Potato",            VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_pulse",               "CropFraction-Pulse",              "Crop Fraction - Pulse",             VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_rapeseed",            "CropFraction-Rapeseed",           "Crop Fraction - Rapeseed",          VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_rice",                "CropFraction-Rice",               "Crop Fraction - Rice",              VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_rye",                 "CropFraction-Rye",                "Crop Fraction - Rye",               VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_sorghum",             "CropFraction-Sorghum",            "Crop Fraction - Sorghum",           VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_soybean",             "CropFraction-Soybean",            "Crop Fraction - Soybean",           VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_sugarbeet",           "CropFraction-Sugarbeet",          "Crop Fraction - Sugarbeet",         VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_sugarcane",           "CropFraction-Sugarcane",          "Crop Fraction - Sugarcane",         VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_sunflower",           "CropFraction-Sunflower",          "Crop Fraction - Sunflower",         VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_fraction_wheat",               "CropFraction-Wheat",              "Crop Fraction - Wheat",             VDBcontinuous, VDBgrey,          VDBavg},
        {"energy",        "longwave_clear_sky_radiation",      "Radiation-LongWave-ClearSky",     "Clear Sky Long Wave Radiation",     VDBcontinuous, VDBgrey,          VDBavg},
        {"energy",        "longwave_downwelling_radiation",    "Radiation-LongWave-Downwelling",  "Downwelling Long Wave Radiation",   VDBcontinuous, VDBgrey,          VDBavg},
        {"energy",        "longwave_upwelling_radiation",      "Radiation-LongWave-Upwelling",    "Upwelling Long Wave Radiation",     VDBcontinuous, VDBgrey,          VDBavg},
        {"energy",        "shortwave_clear_sky_radiation",     "Radiation-ShortWave-ClearSky",    "Clear Sky Short Wave Radiation",    VDBcontinuous, VDBgrey,          VDBavg},
        {"energy",        "shortwave_downwelling_radiation",   "Radiation-ShortWave-Downwelling", "Downwelling Short Wave Radiation",  VDBcontinuous, VDBgrey,          VDBavg},
        {"energy",        "shortwave_upwelling_radiation",     "Radiation-ShortWave-Upwelling",   "Upwelling Short Wave Radiation",    VDBcontinuous, VDBgrey,          VDBavg},
        {"hydrography",   "daily_precipitation_fraction",      "Precipitation-DailyFraction",     "Daily Precipitation Fraction",      VDBcontinuous, VDBgrey,          VDBavg},
        {"hydrography",   "discharge",                         "Discharge",                       "Discharge",                         VDBcontinuous, VDBblue,          VDBavg},
        {"hydrography",   "evapotranspiration",                "Evapotranspiration",              "Evapotranspiration",                VDBcontinuous, VDBgrey,          VDBsum},
        {"hydrography",   "potential_evapotranspiration",      "PotEvapotranspiration",           "Potential Evapotranspiration",      VDBcontinuous, VDBgrey,          VDBsum},
        {"hydrography",   "precipitation",                     "Precipitation",                   "Precipitation",                     VDBcontinuous, VDBblue,          VDBsum},
        {"hydrography",   "rainfall",                          "Rainfall",                        "Rainfall",                          VDBcontinuous, VDBblue,          VDBsum},
        {"hydrography",   "rain_pet",                          "RainPET",                         "Rain Potential ET",                 VDBcontinuous, VDBblue,          VDBsum},
        {"hydrography",   "runoff",                            "Runoff",                          "Runoff",                            VDBcontinuous, VDBblue,          VDBavg},
        {"hydrography",   "snowfall",                          "SnowFall",                        "Snowfall",                          VDBcontinuous, VDBblue,          VDBsum},
        {"hydrography",   "soil_moisture",                     "SoilMoisture",                    "Soil Moisture",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"hydrography",   "soil_moisture_change",              "SoilMoistureChange",              "Soil Moisture Change",              VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "barren_percentage",                 "Landcover-PctBarren",             "Barren Percentage",                 VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "deciduous_broadleaf_percentage",    "Landcover-PctBroadleafDeciduous", "Deciduous Broadleaf Percentage",    VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "evergreen_broadleaf_percentage",    "Landcover-PctBroadleafEvergreen", "Evergreen Broadleaf Percentage",    VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "deciduous_conifer_percentage",      "Landcover-PctConiferDeciduous",   "Deciduous Conifer Percentage",      VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "evergreen_conifer_percentage",      "Landcover-PctConiferEvergreen",   "Evergreen Conifer Percentage",      VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "cropland_percentage",               "Landcover-PctCropland",           "Cropland Percentage",               VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "crop-natural_mosaic_percentage",    "Landcover-PctCropNaturalMosaic",  "Cropland-Natural Mosaic Percentage",VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "grassland_percentage",              "Landcover-PctGrassland",          "Grassland Percentage",              VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "mixed_forest_percentage",           "Landcover-PctMixedForest",        "Mixed Forest Percentage",           VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "mixed_savannah_percentage",         "Landcover-PctSavannah",           "Savannah Percentage",               VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "mixed_woody_savannah_percentage",   "Landcover-PctSavannahWoody",      "Woody Savannah Percentage",         VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "mixed_closed_shrubland_percentage", "Landcover-PctShrublandClosed",    "Closed Shrubland Percentage",       VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "mixed_open_shrubland_percentage",   "Landcover-PctShrublandOpen",      "Open Shrubland Percentage",         VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "mixed_snow_ice_percentage",         "Landcover-PctSnowIce",            "Snow-Ice Percentage",               VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "urban_built-up_percentage",         "Landcover-PctUrbanBuiltUp",       "Urban Built-Up Percentage",         VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "vegetation_percentage",             "Landcover-PctVegetation",         "Vegetation Fraction",               VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "water_percentage",                  "Landcover-PctWater",              "Water Percentage",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"land_cover",    "wetland_percentage",                "Landcover-PctWetland",            "Wetland Percentage",                VDBcontinuous, VDBgrey,          VDBavg},
        {"meteorology",   "air_temperature",                   "AirTemperature",                  "Air Temperature",                   VDBcontinuous, VDBblue2red,      VDBavg},
        {"meteorology",   "min_air_temperature",               "AirTemperature-Min",              "Minimum Air Temperature",           VDBcontinuous, VDBblue2red,      VDBavg},
        {"meteorology",   "max_air_temperature",               "AirTemperature-Max",              "Maximum Air Temperature",           VDBcontinuous, VDBblue2red,      VDBavg},
        {"meteorology",   "diurnal_air_temperature_range",     "AirTemperature-Range",            "Diurnal AirTemperature Range",      VDBcontinuous, VDBgrey,          VDBavg},
        {"meteorology",   "cloud_cover",                       "CloudCover",                      "Cloud Cover",                       VDBcontinuous, VDBgrey,          VDBavg},
        {"meteorology",   "heatindex",                         "Heatindex",                       "Heatindex",                         VDBcontinuous, VDBblue2red,      VDBavg},
        {"meteorology",   "max_heatindex",                     "Heatindex-Max",                   "Maximum Heatindex",                 VDBcontinuous, VDBblue2red,      VDBavg},
        {"meteorology",   "min_heatindex",                     "Heatindex-Min",                   "Minimum Heatindex",                 VDBcontinuous, VDBblue2red,      VDBavg},
        {"meteorology",   "dewpoint_temperature",              "Humidity-Dewpoint",               "Dewpoint Temperature",              VDBcontinuous, VDBblue2red,      VDBavg},
        {"meteorology",   "relative_humidity",                 "Humidity-Relative",               "Relative Humidity",                 VDBcontinuous, VDBblue,          VDBavg},
        {"meteorology",   "specific_humidity",                 "Humidity-Specific",               "Specific Humidity",                 VDBcontinuous, VDBblue,          VDBavg},
        {"meteorology",   "vapor_pressure",                    "Humidity-VaporPressure",          "Vapor Pressure",                    VDBcontinuous, VDBblue,          VDBavg},
        {"meteorology",   "surface_air_pressure",              "AirPressure",                     "Air Pressure",                      VDBcontinuous, VDBgrey,          VDBavg},
        {"meteorology",   "v_wind_speed",                      "WindSpeed-V10m",                  "Wind Speed V-direction",            VDBcontinuous, VDBgrey,          VDBavg},
        {"meteorology",   "wind_speed",                        "WindSpeed",                       "Wind Speed",                        VDBcontinuous, VDBgrey,          VDBavg},
        {"meteorology",   "u_wind_speed",                      "WindSpeed-U10m",                  "Wind Speed U-direction",            VDBcontinuous, VDBgrey,          VDBavg},
        {"network",       "basin",                             "Basin",                           "Basin",                             VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"network",       "mouth",                             "Mouth",                           "Mouth",                             VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"network",       "cell_area",                         "CellArea",                        "Cell Area",                         VDBcontinuous, VDBgrey,          VDBmax},
        {"network",       "cell_slope",                        "CellSlope",                       "Cell Slope",                        VDBcontinuous, VDBgrey,          VDBavg},
        {"network",       "confluence",                        "Confluence",                      "Confluence",                        VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"network",       "distance_to_ocean",                 "DistToOcean",                     "Distance to Ocean",                 VDBcontinuous, VDBgrey,          VDBmax},
        {"network",       "network",                           "Network",                         "STNetwork",                         VDBnetwork,    VDBnotApplicable, VDBnotApplicable},
        {"network",       "ocean_basin",                       "OceanBasin",                      "Ocean Basin",                       VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"network",       "streamline",                        "Streamline",                      "Streamline",                        VDBline,       VDBnotApplicable, VDBnotApplicable},
        {"network",       "subbasin",                          "Subbasin",                        "Subbasin",                          VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"network",       "travel_time",                       "TravelTime",                      "Travel Time",                       VDBcontinuous, VDBgrey,          VDBmax},
        {"network",       "upstream_area",                     "UpstreamArea",                    "Upstream Area",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"reservoir",     "reservoir_dam",                     "Reservoir-Dam",                   "Reservoir Dam",                     VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"reservoir",     "reservoir_area",                    "Reservoir-Area",                  "Reservoir Area",                    VDBcontinuous, VDBgrey,          VDBsum},
        {"reservoir",     "reservoir_capacity",                "Reservoir-Capacity",              "Reservoir Capacity",                VDBcontinuous, VDBgrey,          VDBsum},
        {"reservoir",     "reservoir_height",                  "Reservoir-Height",                "Reservoir Height",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"reservoir",     "reservoir_subbasin",                "Reservoir-Subbasin",              "Reservoir Subbasin",                VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"reservoir",     "reservoir_volume",                  "Reservoir-Volume",                "Reservoir Volume",                  VDBcontinuous, VDBgrey,          VDBsum},
        {"reservoir",     "small_reservoir_coefficient",       "SmallReservoir-Coefficient",      "Small-reservoir Coefficient",       VDBcontinuous, VDBgrey,          VDBavg},
        {"socio_economy", "population",                        "Population",                      "Population",                        VDBcontinuous, VDBgrey,          VDBsum},
        {"socio_economy", "population_density",	              "Population-Density",              "Population Density",                 VDBcontinuous, VDBgrey,          VDBavg},
        {"socio_economy", "population-rural",                  "Population-Rural",                "Rural Population",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"socio_economy", "population-urban",                  "Population-Urban",                "Urban Population",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "available_water_capacity",          "Soil-AvailableWaterCapacity",     "Avaible Water Capacity",            VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "bulk_density",                      "Soil-BulkDensity",                "Bulk Density",                      VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "depth_to_horizon",                  "Soil-DepthToRHorizon",            "Depth to R Horizon",                VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "depth_to_bedrock",                  "Soil-DepthToBedrock",             "Depth to Bedrock",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "field_capacity",                    "Soil-FieldCapacity",              "Field Capacity",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "clay_percent_by_volume",            "Soil-ClayPercentByVolume",        "Percent Clay by Volume",            VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "clay_percent_by_mass",              "Soil-ClayPercentByMass",          "Percent Clay by Mass",              VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "coarse_percent_by_volume",          "Soil-CoarsePercentByVolume",      "Percent Coarse by Volume",          VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "coarse_percent_by_mass",            "Soil-CoarsePercentByMass",        "Percent Coarse by Mass",            VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "particle_density",                  "Soil-ParticleDensity",            "Particle Density",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "porosity",                          "Soil-Porosity",                   "Porosity",                          VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "probability_of_horizon",            "Soil-ProbabilityOfRHorizon",      "Probability of R Horizon",          VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "rooting_depth",                     "Soil-RootingDepth",               "Rooting Depth",                     VDBcontinuous, VDBgrey,          VDBsum},
        {"soil",          "sand_percent_by_volume",            "Soil-SandPercentByVolume",        "Percent Sand by Volume",            VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "sand_percent_by_mass",              "Soil-SandPercentByMass",          "Percent Sand by Mass",              VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "silt_percent_by_volume",            "Soil-SiltPercentByVolume",        "Percent Silt by Volume",            VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "silt_percent_by_mass",              "Soil-SiltPercentByMass",          "Percent Silt by Mass",              VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "wilting_point",                     "Soil-WiltingPoint",               "Wilting Point",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"geopgraphy",    "continent",                         "Continent",                       "Continent",                         VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"geopgraphy",    "continent_mask",                    "ContinentMask",                   "Continent Mask",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"geopgraphy",    "county",                            "County",                          "County",                            VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"geopgraphy",    "county_crossing",                   "CountyCrossing",                  "County Crossing",                   VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"geopgraphy",    "country",                           "Country",                         "Country",                           VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"geopgraphy",    "country_crossing",                  "CountryCrossing",                 "Country Crossing",                  VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"geopgraphy",    "state",                             "State",                           "State",                             VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"geopgraphy",    "state_crossing",                    "StateCrossing",                   "State Crossing",                    VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"geopgraphy",    "hydro_unit",                        "Hydro-Unit",                      "Hydrological Unit",                 VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"geography",     "marine_region",                     "MarineRegion",                    "Marine Region",                     VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"geography",     "ocean",                             "Ocean",                           "Ocean",                             VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"station",       "discharge_gauge",                   "Discharge-Gauge",                 "Discharge Gauge",                   VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"station",       "precipitation_gauge",               "Precipitation-Gauge",             "Precipitation Gauge",               VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"station",       "power_plant",                       "PowerPlant",                      "Power Plant Catchment",             VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"topography",    "bathymetry",                        "Bathymetry",                      "Bathymetry",                        VDBcontinuous, VDBblue,          VDBavg},
        {"topography",    "bathymetry_min",                    "Bathymetry-Min",                  "Minimum Depth",                     VDBcontinuous, VDBblue,          VDBavg},
        {"topography",    "bathymetry_max",                    "Bathymetry-Max",                  "Maximum Depth",                     VDBcontinuous, VDBblue,          VDBavg},
        {"topography",    "elevation",                         "Elevation",                       "Elevation",                         VDBcontinuous, VDBelevation,     VDBavg},
        {"topography",    "min_elevation",                     "Elevation-Min",                   "Minimum Elevation",                 VDBcontinuous, VDBelevation,     VDBavg},
        {"topography",    "max_elevation",                     "Elevation-Max",                   "Maximum Elevation",                 VDBcontinuous, VDBelevation,     VDBavg}};

static VDBmasterTable_t _VDBmasterTable3 [] = {
        {"biochemistry",  "biochem_c_litterfall",                         "Biochem-C-LitterFall",                         "C LitterFall",                        VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "biochem_din_areal_loading",                    "Biochem-DIN-AreaLoading",                      "DIN Areal Loading",                   VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "biochem_din_point_loading",                    "Biochem-DIN-PointLoading",                     "DIN Point Loading",                   VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "biochem_doc_areal_loading",                    "Biochem-DOC-ArealLoading",                     "DOC Areal Loading",                   VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "biochem_doc_point_loading",                    "Biochem-DOC-PointLoading",                     "DOC Point Loading",                   VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "biochem_don_areal_loading",                    "Biochem-DON-ArealLoading",                     "DON Areal Loading",                   VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "biochem_don_point_loading",                    "Biochem-DON-PointLoading",                     "DON Point Loading",                   VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "biochem_n_litterfall",                         "Biochem-N-LitterFall",                         "N LitterFall",                        VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "biochem_nh4_areal_loading",                    "Biochem-NH4-ArealLoading",                     "NH4 Areal Loading",                   VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "biochem_nh4_point_loading",                    "Biochem-Nh4-PointLoading",                     "NH4 Point Loading",                   VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "biochem_no3_areal_loading",                    "Biochem-NO3-ArealLoading",                     "NO3 Areal Loading",                   VDBcontinuous, VDBgrey,          VDBsum},
        {"biochemistry",  "biochem_no3_point_loading",                    "Biochem-NO3-PointLoading",                     "NO3 Point Loading",                   VDBcontinuous, VDBgrey,          VDBsum},
        {"crop",          "crop_growing_season1",                         "Crop-GrowingSeason1",                          "Growing Season 1",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_growing_season2",                         "Crop-GrowingSeason2",                          "Growing Season 2",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated",                               "CropIrrigated",                                "Irrigated Crops",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_intensity",                     "Crop-Irrigated-Intensity",                     "Irrigated Cropping Intensity",        VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_efficiency",                    "Crop-Irrigated-Efficiency",                    "Irrigation Efficiency",               VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_gross_demand",                  "Crop-Irrigated-GrossDemand",                   "Irrigation Gross Water Demand",       VDBcontinuous, VDBgrey,          VDBsum},
        {"crop",          "crop_irrigated_net_demand",                    "Crop-Irrigated-NetDemand",                     "Irrigation Net Water Demand",         VDBcontinuous, VDBgrey,          VDBsum},
        {"crop",          "crop_irrigated_return_flow",                   "Crop-Irrigated-ReturnFlow",                    "Irrigation Return Flow",              VDBcontinuous, VDBgrey,          VDBsum},
        {"crop",          "crop_irrigated_rice_percolation_rate" ,        "Crop-Irrigated-RicePercolationRate",           "Rice Percolation Rate",               VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_banana",                        "CropIrrigated-Banana",                         "Irrigated Banana",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_barley",                        "CropIrrigated-Barley",                         "Irrigated Barley",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_cassava",                       "CropIrrigated-Cassava",                        "Irrigated Cassava",                   VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_cereals",                       "CropIrrigated-Cereals",                        "Irrigated Cereals",                   VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_cotton",                        "CropIrrigated-Cotton",                         "Irrigated Cotton",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_fodder",                        "CropIrrigated-Fodder",                         "Irrigated Fodder",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_groundnut",                     "CropIrrigated-Groundnut",                      "Irrigated Groundnut",                 VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_maize",                         "CropIrrigated-Maize",                          "Irrigated Maize",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_millet",                        "CropIrrigated-Millet",                         "Irrigated Millet",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_nes",                           "CropIrrigated-NES",                            "Irrigated NES",                       VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_oilpalm",                       "CropIrrigated-Oilpalm",                        "Irrigated Oilpalm",                   VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_potato",                        "CropIrrigated-Potato",                         "Irrigated Potato",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_pulse",                         "CropIrrigated-Pulse",                          "Irrigated Pulse",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_rapeseed",                      "CropIrrigated-Rapeseed",                       "Irrigated Rapeseed",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_rice",                          "CropIrrigated-Rice",                           "Irrigated Rice",                      VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_rye",                           "CropIrrigated-Rye",                            "Irrigated Rye",                       VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_sorghum",                       "CropIrrigated-Sorghum",                        "Irrigated Sorghum",                   VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_soybean",                       "CropIrrigated-Soybean",                        "Irrigated Soybean",                   VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_stimulant",                     "CropIrrigated-Stimulant",                      "Irrigated Stimulant",                 VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_sugarbeet",                     "CropIrrigated-Sugarbeet",                      "Irrigated Sugarbeet",                 VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_sugarcane",                     "CropIrrigated-Sugarcane",                      "Irrigated Sugarcane",                 VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_sunflower",                     "CropIrrigated-Sunflower",                      "Irrigated Sunflower",                 VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_tobacco",                       "CropIrrigated-Tobacco",                        "Irrigated Tobacco",                   VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_vegetables",                    "CropIrrigated-Vegetables",                     "Irrigated Vegetables",                VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_wheat",                         "CropIrrigated-Wheat",                          "Irrigated Wheat",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_yamsan",                        "CropIrrigated-Yasman",                         "Irrigated Yasman and other roots",    VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_other",                         "CropIrrigated-Other",                          "Irrigated Other",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_irrigated_perennial",                     "CropIrrigated-Perennial",                      "Irrigated Perennial",                 VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed",                                 "CropRainfed",                                  "Rainfed Crops",                       VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_intensity",                       "CropRainfed-Intensity",                        "Rainfed Cropping Intensity",          VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_banana",                          "CropRainfed-Banana",                           "Rainfed Banana",                      VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfef_barley",                          "CropRainfed-Barley",                           "Rainfed Barley",                      VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_cassava",                         "CropRainfed-Cassava",                          "Rainfed Cassava",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_cereals",                         "CropRainfed-Cereals",                          "Rainfed Cereals",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_cotton",                          "CropRainfed-Cotton",                           "Rainfed Cotton",                      VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_fodder",                          "CropRainfed-Fodder",                           "Rainfed Fodder",                      VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_groundnut",                       "CropRainfed-Groundnut",                        "Rainfed Groundnut",                   VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_maize",                           "CropRainfed-Maize",                            "Rainfed Maize",                       VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_millet",                          "CropRainfed-Millet",                           "Rainfed Millet",                      VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_nes",                             "CropRainfed-NES",                              "Rainfed NES",                         VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_oilpalm",                         "CropRainfed-Oilpalm",                          "Rainfed Oilpalm",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_potato",                          "CropRainfed-Potato",                           "Rainfed Potato",                      VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_pulse",                           "CropRainfed-Pulse",                            "Rainfed Pulse",                       VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_rapeseed",                        "CropRainfed-Rapeseed",                         "Rainfed Rapeseed",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_rice",                            "CropRainfed-Rice",                             "Rainfed Rice",                        VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_rye",                             "CropRainfed-Rye",                              "Rainfed Rye",                         VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_sorghum",                         "CropRainfed-Sorghum",                          "Rainfed Sorghum",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_soybean",                         "CropRainfed-Soybean",                          "Rainfed Soybean",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_stimulant",                       "CropRainfed-Stimulant",                        "Rainfed Stimulant",                   VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_sugarbeet",                       "CropRainfed-Sugarbeet",                        "Rainfed Sugarbeet",                   VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_sugarcane",                       "CropRainfed-Sugarcane",                        "Rainfed Sugarcane",                   VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_sunflower",                       "CropRainfed-Sunflower",                        "Rainfed Sunflower",                   VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_tobacco",                         "CropRainfed-Tobacco",                          "Rainfed Tobacco",                     VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_vegetables",                      "CropRainfed-Vegetables",                       "Rainfed Vegetables",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_wheat",                           "CropRainfed-Wheat",                            "Rainfed Wheat",                       VDBcontinuous, VDBgrey,          VDBavg},
        {"crop",          "crop_rainfed_yamsan",                          "CropRainfed-Yasman",                           "Rainfed Yasman and other roots",      VDBcontinuous, VDBgrey,          VDBavg},
        {"geography",     "geography_continent",                          "Geography-Continent",                          "Continent",                           VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"geography",     "geography_county",                             "Geography-County",                             "County",                              VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"geography",     "geography_county_crossing",                    "Geography-CountyCrossing",                     "County Crossing",                     VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"geography",     "geography_country",                            "Geography-Country",                            "Country",                             VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"geography",     "geography_country_crossing",                   "Geography-CountryCrossing",                    "Country Crossing",                    VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"geography",     "geography_state",                              "Geography-State",                              "State",                               VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"geography",     "geography_state_crossing",                     "Geography-StateCrossing",                      "State Crossing",                      VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"geography",     "geography_landmask",                           "Geography-Landmask",                           "Landmask",                            VDBcontinuous, VDBgrey,          VDBavg},
        {"geography",     "geography_hydrological_unit",                  "Geography-HydrologicalUnit",                   "Hydrological Unit",                   VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"geography",     "geography_hydrological_unit_crossing",         "Geography-HydrologicalUnitCrossing",           "Hydrological Unit Crossing",          VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"geography",     "geography_marine_region",                      "Geography-MarineRegion",                       "Marine Region",                       VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"geography",     "geography_ocean",                              "Geography-Ocean",                              "Ocean",                               VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"hydrography",   "hydrography_discharge",                        "Hydrography-Discharge",                        "Discharge",                           VDBcontinuous, VDBblue,          VDBavg},
        {"hydrography",   "hydrography_drought_index_palmer",             "Hydrography-DroughtIndex-Palmer",              "Palmer Drought Index",                VDBcontinuous, VDBblue,          VDBavg},
        {"hydrography",   "hydrography_evapotranspiration",               "Hydrography-Evapotranspiration",               "Evapotranspiration",                  VDBcontinuous, VDBgrey,          VDBsum},
        {"hydrography",   "hydrography_evapotranspiration_deficit",       "Hydrography-Evapotranspiration-Deficit",       "Water Deficit",                       VDBcontinuous, VDBgrey,          VDBsum},
        {"hydrography",   "hydrography_evapotranspiration_potential",     "Hydrography-Evapotranspiration-Potential",     "Potential Evapotranspiration",        VDBcontinuous, VDBgrey,          VDBsum},
        {"hydrography",   "hydrography_evapotranspiration_potential_land","Hydrography-Evapotranspiration-PotentialLand", "Potenital Evapotranspiration - Land", VDBcontinuous, VDBblue,          VDBsum},
        {"hydrography",   "hydrography_evapotranspiration_potential_crop","Hydrography-Evapotranspiration-PotentialCrop", "Potential Evapotranspiration - Crop", VDBcontinuous, VDBblue,          VDBsum},
        {"hydrography",   "hydrography_precipitation",                    "Hydrography-Precipitation",                    "Precipitation",                       VDBcontinuous, VDBblue,          VDBsum},
        {"hydrography",   "hydrography_precipitation_daily_fraction",     "Hydrography-Precipitation-DailyFraction",      "Daily Precipitation Fraction",        VDBcontinuous, VDBgrey,          VDBavg},
        {"hydrography",   "hydrography_precipitation_rainfall",           "Hydrography-Precipitation-Rainfall",           "Precipitation",                       VDBcontinuous, VDBblue,          VDBsum},
        {"hydrography",   "hydrography_precipitation_snowfall",           "Hydrography-Precipitation-Snowfall",           "Snowfall",                            VDBcontinuous, VDBblue,          VDBsum},
        {"hydrography",   "hydrography_runoff",                           "Hydrography-Runoff",                           "Runoff",                              VDBcontinuous, VDBblue,          VDBavg},
        {"hydrography",   "hydrography_runoff_surface",                   "Hydrography-Runoff-Surface",                   "Surface Runoff",                      VDBcontinuous, VDBblue,          VDBavg},
        {"hydrography",   "hydrography_runoff_subsurface",                "Hydrography-Runoff-Subsurface",                "Subsurface Runoff",                   VDBcontinuous, VDBblue,          VDBavg},
        {"hydrography",   "hydrography_snow_water_equivalent",            "Hydrography-Snow-WaterEquivalent",             "Snow Water Equivalent",               VDBcontinuous, VDBblue,          VDBavg},
        {"hydrography",   "hydrography_soil_moisture",                    "Hydrography-SoilMoisture",                     "Soil Moisture",                       VDBcontinuous, VDBgrey,          VDBavg},
        {"hydrography",   "hydrography_soil_moisture_change",             "Hydrography-SoilMoisture-Change",              "Soil Moisture Change",                VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_crop",                                    "LandFraction_Crop",                            "Crop Area Fraction",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_barren",                                  "LandFraction-Barren",                          "Barren Area Fraction",                VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_vegetation",                              "LandFraction-Vegetation",                      "Vegetation Area Area Fraction",       VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_vegetation_deciduous_broadleaf",          "LandFraction-Vegetation-BroadleafDeciduous",   "Deciduous Broadleaf Area Fraction",   VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_vegetation_evergreen_broadleaf",          "LandFraction-Vegetation-BroadleafEvergreen",   "Evergreen Broadleaf Area Fraction",   VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_vegetation_deciduous_conifer",            "LandFraction-Vegetation-ConiferDeciduous",     "Deciduous Conifer Area Fraction",     VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_vegetation_evergreen_conifer",            "LandFraction-Vegetation-ConiferEvergreen",     "Evergreen Conifer Area Fraction",     VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_vegetation_grassland",                    "LandFraction-Vegetation-Grassland",            "Grassland Area Fraction",             VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_vegetation_mixed_forest",                 "LandFraction-Vegetation-MixedForest",          "Mixed Forest Area Fraction",          VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_vegetation_mixed_savannah",               "LandFraction-Vegetation-Savannah",             "Savannah Area Fraction",              VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_vegetation_mixed_woody_savannah",         "LandFraction-Vegetation-SavannahWoody",        "Woody Savannah Area Fraction",        VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_vegetation_mixed_closed_shrubland",       "LandFraction-Vegetation-ShrublandClosed",      "Closed Shrubland Area Fraction",      VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_vegetation_mixed_open_shrubland",         "LandFraction-Vegetation-ShrublandOpen",        "Open Shrubland Area Fraction",        VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_snow_ice",                                "LandFraction-SnowIce",                         "Snow-Ice Area Fraction",              VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_urban_built-up",                          "LandFraction-UrbanBuiltUp",                    "Urban Built-Up Area Fraction",        VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_water",                                   "LandFraction-Water",                           "Water Area Fraction",                 VDBcontinuous, VDBgrey,          VDBavg},
        {"land_fraction", "land_wetland",                                 "LandFraction-Wetland",                         "Wetland Area Fraction",               VDBcontinuous, VDBgrey,          VDBavg},
        {"radiation",     "radiation_longwave_downwelling",               "Radiation-LongWave-Downwelling",               "Downwelling Long Wave Radiation",     VDBcontinuous, VDBgrey,          VDBavg},
        {"radiation",     "radiation_longwave_upwelling",                 "Radiation-LongWave-Upwelling",                 "Upwelling Long Wave Radiation",       VDBcontinuous, VDBgrey,          VDBavg},
        {"radiation",     "radiation_shortwave_clear_sky",                "Radiation-ShortWave-ClearSky",                 "Clear Sky Short Wave Radiation",      VDBcontinuous, VDBgrey,          VDBavg},
        {"radiation",     "radiation_shortwave_downwelling",              "Radiation-ShortWave-Downwelling",              "Downwelling Short Wave Radiation",    VDBcontinuous, VDBgrey,          VDBavg},
        {"radiation",     "radiation_shortwave_upwelling",                "Radiation-ShortWave-Upwelling",                "Upwelling Short Wave Radiation",      VDBcontinuous, VDBgrey,          VDBavg},
        {"reservoir",     "reservoir_area",                               "Reservoir-Area",                               "Reservoir Area",                      VDBcontinuous, VDBgrey,          VDBsum},
        {"reservoir",     "reservoir_capacity",                           "Reservoir-Capacity",                           "Reservoir Capacity",                  VDBcontinuous, VDBgrey,          VDBsum},
        {"reservoir",     "reservoir_dam",                                "Reservoir-Dam",                                "Reservoir Dam",                       VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"reservoir",     "reservoir_height",                             "Reservoir-Height",                             "Reservoir Height",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"reservoir",     "reservoir_subbasin",                           "Reservoir-Subbasin",                           "Reservoir Subbasin",                  VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"reservoir",     "reservoir_volume",                             "Reservoir-Volume",                             "Reservoir Volume",                    VDBcontinuous, VDBgrey,          VDBsum},
        {"reservoir",     "reservoir_small_coefficient",                  "Reservoir-Small-Coefficient",                  "Small-reservoir Coefficient",         VDBcontinuous, VDBgrey,          VDBavg},
        {"river",         "river_basin",                                  "River-Basin",                                  "River Basin",                         VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"river",         "river_mouth",                                  "River-Mouth",                                  "River Mouth",                         VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"river",         "river_cell_area",                              "River-CellArea",                               "River Cell Area",                     VDBcontinuous, VDBgrey,          VDBmax},
        {"river",         "river_cell_slope",                             "River-CellSlope",                              "River Cell Slope",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"river",         "river_confluence",                             "River-Confluence",                             "River Confluence",                    VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"river",         "river_corridor",                               "River-Corridor",                               "River Corridor",                      VDBline,       VDBnotApplicable, VDBnotApplicable},
        {"river",         "river_distance_to_ocean",                      "River-DistToOcean",                            "Distance to Ocean",                   VDBcontinuous, VDBgrey,          VDBmax},
        {"river",         "river_network",                                "River-Network",                                "STNetwork",                           VDBnetwork,    VDBnotApplicable, VDBnotApplicable},
        {"river",         "river_ocean_basin",                            "River-OceanBasin",                             "Ocean Basin",                         VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"river",         "river_streamline",                             "River-Streamline",                             "Streamline",                          VDBline,       VDBnotApplicable, VDBnotApplicable},
        {"river",         "river_subbasin",                               "River-Subbasin",                               "Subbasin",                            VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"river",         "river_travel_time",                            "River-TravelTime",                             "Travel Time",                         VDBcontinuous, VDBgrey,          VDBmax},
        {"river",         "river_upstream_area",                          "River-UpstreamArea",                           "Upstream Area",                       VDBcontinuous, VDBgrey,          VDBavg},
        {"socioeconomy",  "socioeconomy_energy_regions",                  "Socioeconomy-Energy-Regions",                  "Energy Regions",                      VDBdiscrete,   VDBnotApplicable, VDBnotApplicable},
        {"socioeconomy",  "socioeconomy_population",                      "Socioeconomy-Population",                      "Population",                          VDBcontinuous, VDBgrey,          VDBsum},
        {"socioeconomy",  "socioeconomy_population_density",	          "Socioeconomy-Population-Density",              "Population Density",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"socioeconomy",  "socioeconomy_population-rural",                "Socioeconomy-Population-Rural",                "Rural Population",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"socioeconomy",  "socioeconomy_population-urban",                "Socioeconomy-Population-Urban",                "Urban Population",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_available_water_capacity",                "Soil-AvailableWaterCapacity",                  "Avaible Water Capacity",              VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_bulk_density",                            "Soil-BulkDensity",                             "Bulk Density",                        VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_depth_to_horizon",                        "Soil-DepthToRHorizon",                         "Depth to R Horizon",                  VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_depth_to_bedrock",                        "Soil-DepthToBedrock",                          "Depth to Bedrock",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_field_capacity",                          "Soil-FieldCapacity",                           "Field Capacity",                      VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_clay_percent_by_volume",                  "Soil-ClayPercentByVolume",                     "Percent Clay by Volume",              VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_clay_percent_by_mass",                    "Soil-ClayPercentByMass",                       "Percent Clay by Mass",                VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_coarse_percent_by_volume",                "Soil-CoarsePercentByVolume",                   "Percent Coarse by Volume",            VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_coarse_percent_by_mass",                  "Soil-CoarsePercentByMass",                     "Percent Coarse by Mass",              VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_particle_density",                        "Soil-ParticleDensity",                         "Particle Density",                    VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_porosity",                                "Soil-Porosity",                                "Porosity",                            VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_probability_of_horizon",                  "Soil-ProbabilityOfRHorizon",                   "Probability of R Horizon",            VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_rooting_depth",                           "Soil-RootingDepth",                            "Rooting Depth",                       VDBcontinuous, VDBgrey,          VDBsum},
        {"soil",          "soil_sand_percent_by_volume",                  "Soil-SandPercentByVolume",                     "Percent Sand by Volume",              VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_sand_percent_by_mass",                    "Soil-SandPercentByMass",                       "Percent Sand by Mass",                VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_silt_percent_by_volume",                  "Soil-SiltPercentByVolume",                     "Percent Silt by Volume",              VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_silt_percent_by_mass",                    "Soil-SiltPercentByMass",                       "Percent Silt by Mass",                VDBcontinuous, VDBgrey,          VDBavg},
        {"soil",          "soil_wilting_point",                           "Soil-WiltingPoint",                            "Wilting Point",                       VDBcontinuous, VDBgrey,          VDBavg},
        {"station",       "station_discharge",                            "Station-Discharge",                            "Discharge Gauge",                     VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"station",       "station_precipitation",                        "Station-Precipitation",                        "Precipitation Gauge",                 VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"station",       "station_power_plant",                          "Station-PowerPlant",                           "Power Plant Catchment",               VDBpoint,      VDBnotApplicable, VDBnotApplicable},
        {"topography",    "topography_bathymetry",                        "Topography-Bathymetry",                        "Bathymetry",                          VDBcontinuous, VDBblue,          VDBavg},
        {"topography",    "topography_bathymetry-min",                    "Topography-Bathymetry-Min",                    "Minimum Depth",                       VDBcontinuous, VDBblue,          VDBavg},
        {"topography",    "topography_bathymetry-max",                    "Topography-Bathymetry-Max",                    "Maximum Depth",                       VDBcontinuous, VDBblue,          VDBavg},
        {"topography",    "topography_elevation",                         "Topography-Elevation",                         "Elevation",                           VDBcontinuous, VDBelevation,     VDBavg},
        {"topography",    "topography_elevation-min",                     "Topography-Elevation-Min",                     "Minimum Elevation",                   VDBcontinuous, VDBelevation,     VDBavg},
        {"topography",    "topography_elevation-max",                     "Topography-Elevation-Max",                     "Maximum Elevation",                   VDBcontinuous, VDBelevation,     VDBavg},
        {"weather",       "weather_air_temperature",                      "Weather-AirTemperature",                       "Air Temperature",                     VDBcontinuous, VDBblue2red,      VDBavg},
        {"weather",       "weather_air_temperature-min",                  "Weather-AirTemperature-Min",                   "Minimum Air Temperature",             VDBcontinuous, VDBblue2red,      VDBavg},
        {"weather",       "weather_air_temperature-max",                  "Weather-AirTemperature-Max",                   "Maximum Air Temperature",             VDBcontinuous, VDBblue2red,      VDBavg},
        {"weather",       "weather_air_temperature_diurnal_range",        "Weather-AirTemperature-Range",                 "Diurnal Air Temperature Range",       VDBcontinuous, VDBgrey,          VDBavg},
        {"weather",       "weather_air_pressure",                         "Weather-AirPressure",                          "Air Pressure",                        VDBcontinuous, VDBgrey,          VDBavg},
        {"weather",       "weather_cloud_cover",                          "Weather-CloudCover",                           "Cloud Cover",                         VDBcontinuous, VDBgrey,          VDBavg},
        {"weather",       "weather_heatindex",                            "Weather-Heatindex",                            "Heatindex",                           VDBcontinuous, VDBblue2red,      VDBavg},
        {"weather",       "weather_heatindex-max",                        "Weather-Heatindex-Max",                        "Maximum Heatindex",                   VDBcontinuous, VDBblue2red,      VDBavg},
        {"weather",       "weather_heatindex-min",                        "Weather-Heatindex-Min",                        "Minimum Heatindex",                   VDBcontinuous, VDBblue2red,      VDBavg},
        {"weather",       "weather_humidity_dewpoint",                    "Weather-Humidity-Dewpoint",                    "Dewpoint Temperature",                VDBcontinuous, VDBblue2red,      VDBavg},
        {"weather",       "weather_humidity_relative",                    "Weather-Humidity-Relative",                    "Relative Humidity",                   VDBcontinuous, VDBblue,          VDBavg},
        {"weather",       "weather_humidity_specific",                    "Weather-Humidity-Specific",                    "Specific Humidity",                   VDBcontinuous, VDBblue,          VDBavg},
        {"weather",       "weather_humidity_vapor_pressure",              "Weather-Humidity-VaporPressure",               "Vapor Pressure",                      VDBcontinuous, VDBblue,          VDBavg},
        {"weather",       "weather_humidity_vapor_pressure_deficit",      "Weather-Humidity-VaporPressure-Deficit",       "Vapor Pressure Deficit",              VDBcontinuous, VDBblue2red,      VDBavg},
        {"weather",       "weather_wind_speed",                           "Weather-WindSpeed",                            "Wind Speed",                          VDBcontinuous, VDBgrey,          VDBavg},
        {"weather",       "weather_wind_speed_u10m",                      "Weather-WindSpeed-U10m",                       "Wind Speed U-direction at 10m",       VDBcontinuous, VDBgrey,          VDBavg},
        {"weather",       "weather_wind_speed_v10m",                      "Weather-WindSpeed-V10m",                       "Wind Speed V-direction at 10m",       VDBcontinuous, VDBgrey,          VDBavg}};

#define VDBrecordNum2 sizeof( _VDBmasterTable2)/sizeof(VDBmasterTable_t)
#define VDBrecordNum3 sizeof( _VDBmasterTable3)/sizeof(VDBmasterTable_t)

const char *VDBrgName (VDBversion version, const char *cfName) {
    int i;

    switch (version) {
        case VDBversion2:
            for (i = 0; i < VDBrecordNum2; ++i) {
                if (strcmp (cfName, _VDBmasterTable2[i].CFname) == 0) return (_VDBmasterTable2[i].GHAASname);
            }
            break;
        case VDBversion3:
            for (i = 0; i < VDBrecordNum3; ++i) {
                if (strcmp (cfName, _VDBmasterTable3[i].CFname) == 0) return (_VDBmasterTable3[i].GHAASname);
            }
            break;
    }
    return ((const  char *) NULL);
}

const char *VDBlongName (VDBversion version, const char *cfName) {
    int i;

    switch (version) {
        case VDBversion2:
            for (i = 0; i < VDBrecordNum2; ++i) {
                if (strcmp (cfName, _VDBmasterTable2[i].CFname) == 0) return (_VDBmasterTable2[i].LongName);
            }
            break;
        case VDBversion3:
            for (i = 0; i < VDBrecordNum3; ++i) {
                if (strcmp (cfName, _VDBmasterTable3[i].CFname) == 0) return (_VDBmasterTable3[i].LongName);
            }
            break;
    }
    return ((const  char *) NULL);
}

const char *VDBcfName (VDBversion version, const char *rgName) {
    int i;

    switch (version) {
        case VDBversion2:
            for (i = 0; i < VDBrecordNum2; ++i) {
                if (strcmp (rgName, _VDBmasterTable2[i].GHAASname) == 0) return (_VDBmasterTable2[i].CFname);
            }
            break;
        case VDBversion3:
            for (i = 0; i < VDBrecordNum3; ++i) {
                if (strcmp (rgName, _VDBmasterTable3[i].GHAASname) == 0) return (_VDBmasterTable3[i].CFname);
            }
            break;
    }
    return ((const  char *) NULL);
}

const char *VDBdataType (VDBversion version, const char *cfName) {
    int i;

    switch (version) {
        case VDBversion2:
            for (i = 0; i < VDBrecordNum2; ++i) {
                if (strcmp (cfName, _VDBmasterTable2[i].CFname) == 0) return (_VDBmasterTable2[i].DataType);
            }
            break;
        case VDBversion3:
            for (i = 0; i < VDBrecordNum3; ++i) {
                if (strcmp (cfName, _VDBmasterTable3[i].CFname) == 0) return (_VDBmasterTable3[i].DataType);
            }
            break;
    }
    return ((const  char *) NULL);
}

const char *VDBshadset (VDBversion version, const char *cfName) {
    int i;

    switch (version) {
        case VDBversion2:
            for (i = 0; i < VDBrecordNum2; ++i) {
                if (strcmp (cfName, _VDBmasterTable2[i].CFname) == 0) return (_VDBmasterTable2[i].Shadeset);
            }
            break;
        case VDBversion3:
            for (i = 0; i < VDBrecordNum3; ++i) {
                if (strcmp (cfName, _VDBmasterTable3[i].CFname) == 0) return (_VDBmasterTable3[i].Shadeset);
            }
            break;
    }
    return ((const  char *) NULL);
}

const char *VDBaggregation (VDBversion version, const char *cfName) {
    int i;

    switch (version) {
        case VDBversion2:
            for (i = 0; i < VDBrecordNum2; ++i) {
                if (strcmp(cfName, _VDBmasterTable2[i].CFname) == 0) return (_VDBmasterTable2[i].Aggregation);
            }
            break;
        case VDBversion3:
            for (i = 0; i < VDBrecordNum3; ++i) {
                if (strcmp(cfName, _VDBmasterTable3[i].CFname) == 0) return (_VDBmasterTable3[i].Aggregation);
            }
            break;
    }
    return ((const  char *) NULL);
}
