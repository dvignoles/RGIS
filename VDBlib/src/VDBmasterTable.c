/******************************************************************************

GHAAS Command Line Library V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDBvariableDB.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <string.h>
#include <vdb.h>

static VDBmasterTable_t _VDBmasterTable2 [] = {
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
        {"reservoir",     "reservoir",                         "Reservoir",                       "Reservoir",                         "point",      "N/A",         "N/A"},
        {"reservoir",     "reservoir_area",                    "Reservoir-Area",                  "Reservoir Area",                    "continuous", "grey",        "sum"},
        {"reservoir",     "reservoir_capacity",                "Reservoir-Capacity",              "Reservoir Capacity",                "continuous", "grey",        "sum"},
        {"reservoir",     "reservoir_height",                  "Reservoir-Height",                "Reservoir Height",                  "continuous", "grey",        "avg"},
        {"reservoir",     "reservoir_volume",                  "Reservoir-Volume",                "Reservoir Volume",                  "continuous", "grey",        "sum"},
        {"reservoir",     "small_reservoir_coefficient",       "SmallReservoir-Coefficient",      "Small-reservoir Coefficient",       "continuous", "grey",        "avg"},
        {"socio_economy", "population",                        "Population",                      "Population",                        "continuous", "grey",        "sum"},
        {"socio_economy", "population_density",	              "Population-Density",              "Population Density",                "continuous", "grey",        "avg"},
        {"socio_economy", "population-rural",                  "Population-Rural",                "Rural Population",                  "continuous", "grey",        "avg"},
        {"socio_economy", "population-urban",                  "Population-Urban",                "Urban Population",                  "continuous", "grey",        "avg"},
        {"soil",          "available_water_capacity",          "Soil-AvailableWaterCapacity",     "Avaible Water Capacity",            "continuous", "grey",        "avg"},
        {"soil",          "bulk_density",                      "Soil-BulkDensity",                "Bulk Density",                      "continuous", "grey",        "avg"},
        {"soil",          "depth_to_horizon",                  "Soil-DepthToRHorizon",            "Depth to R Horizon",                "continuous", "grey",        "avg"},
        {"soil",          "depth_to_bedrock",                  "Soil-DepthToBedrock",             "Depth to Bedrock",                  "continuous", "grey",        "avg"},
        {"soil",          "field_capacity",                    "Soil-FieldCapacity",              "Field Capacity",                    "continuous", "grey",        "avg"},
        {"soil",          "clay_percent_by_volume",            "Soil-ClayPercentByVolume",        "Percent Clay by Volume",            "continuous", "grey",        "avg"},
        {"soil",          "clay_percent_by_mass",              "Soil-ClayPercentByMass",          "Percent Clay by Mass",              "continuous", "grey",        "avg"},
        {"soil",          "coarse_percent_by_volume",          "Soil-CoarsePercentByVolume",      "Percent Coarse by Volume",          "continuous", "grey",        "avg"},
        {"soil",          "coarse_percent_by_mass",            "Soil-CoarsePercentByMass",        "Percent Coarse by Mass",            "continuous", "grey",        "avg"},
        {"soil",          "particle_density",                  "Soil-ParticleDensity",            "Particle Density",                  "continuous", "grey",        "avg"},
        {"soil",          "porosity",                          "Soil-Porosity",                   "Porosity",                          "continuous", "grey",        "avg"},
        {"soil",          "probability_of_horizon",            "Soil-ProbabilityOfRHorizon",      "Probability of R Horizon",          "continuous", "grey",        "avg"},
        {"soil",          "rooting_depth",                     "Soil-RootingDepth",               "Rooting Depth",                     "continuous", "grey",        "sum"},
        {"soil",          "sand_percent_by_volume",            "Soil-SandPercentByVolume",        "Percent Sand by Volume",            "continuous", "grey",        "avg"},
        {"soil",          "sand_percent_by_mass",              "Soil-SandPercentByMass",          "Percent Sand by Mass",              "continuous", "grey",        "avg"},
        {"soil",          "silt_percent_by_volume",            "Soil-SiltPercentByVolume",        "Percent Silt by Volume",            "continuous", "grey",        "avg"},
        {"soil",          "silt_percent_by_mass",              "Soil-SiltPercentByMass",          "Percent Silt by Mass",              "continuous", "grey",        "avg"},
        {"soil",          "wilting_point",                     "Soil-WiltingPoint",               "Wilting Point",                     "continuous", "grey",        "avg"},
        {"geopgraphy",    "continent",                         "Continent",                       "Continent",                         "discrete",   "N/A",         "N/A"},
        {"geopgraphy",    "continent_mask",                    "ContinentMask",                   "Continent Mask",                    "continuous", "grey",        "avg"},
        {"geopgraphy",    "county",                            "County",                          "County",                            "discrete",   "N/A",         "N/A"},
        {"geopgraphy",    "county_crossing",                   "CountyCrossing",                  "County Crossing",                   "point",      "N/A",         "N/A"},
        {"geopgraphy",    "country",                           "Country",                         "Country",                           "discrete",   "N/A",         "N/A"},
        {"geopgraphy",    "country_crossing",                  "CountryCrossing",                 "Country Crossing",                  "point",      "N/A",         "N/A"},
        {"geopgraphy",    "state",                             "State",                           "State",                             "discrete",   "N/A",         "N/A"},
        {"geopgraphy",    "state_crossing",                    "StateCrossing",                   "State Crossing",                    "point",      "N/A",         "N/A"},
        {"geopgraphy",    "hydro_unit",                        "Hydro-Unit",                      "Hydrological Unit",                 "discrete",   "N/A",         "N/A"},
        {"station",       "discharge_gauge",                   "Discharge-Gauge",                 "Discharge Gauge",                   "point",      "N/A",         "N/A"},
        {"station",       "precipitation_gauge",               "Precipitation-Gauge",             "Precipitation Gauge",               "point",      "N/A",         "N/A"},
        {"station",       "power_plant",                       "PowerPlant",                      "Power Plant Catchment",             "point",      "N/A",         "N/A"},
        {"topography",    "bathymetry",                        "Bathymetry",                      "Bathymetry",                        "continuous", "blue",        "avg"},
        {"topography",    "bathymetry_min",                    "Bathymetry-Min",                  "Minimum Depth",                     "continuous", "blue",        "avg"},
        {"topography",    "bathymetry_max",                    "Bathymetry-Max",                  "Maximum Depth",                     "continuous", "blue",        "avg"},
        {"topography",    "elevation",                         "Elevation",                       "Elevation",                         "continuous", "elevation",   "avg"},
        {"topography",    "min_elevation",                     "Elevation-Min",                   "Minimum Elevation",                 "continuous", "elevation",   "avg"},
        {"topography",    "max_elevation",                     "Elevation-Max",                   "Maximum Elevation",                 "continuous", "elevation",   "avg"}};

static VDBmasterTable_t _VDBmasterTable3 [] = {
        {"biochemistry",  "biochem_c_litterfall",                              "Biochem-C-LitterFall",                       "C LitterFall",                       "continuous", "grey",        "sum"},
        {"biochemistry",  "biochem_din_areal_loading",                         "Biochem-DIN-AreaLoading",                    "DIN Areal Loading",                  "continuous", "grey",        "sum"},
        {"biochemistry",  "biochem_din_point_loading",                         "Biochem-DIN-PointLoading",                   "DIN Point Loading",                  "continuous", "grey",        "sum"},
        {"biochemistry",  "biochem_doc_areal_loading",                         "Biochem-DOC-ArealLoading",                   "DOC Areal Loading",                  "continuous", "grey",        "sum"},
        {"biochemistry",  "biochem_doc_point_loading",                         "Biochem-DOC-PointLoading",                   "DOC Point Loading",                  "continuous", "grey",        "sum"},
        {"biochemistry",  "biochem_don_areal_loading",                         "Biochem-DON-ArealLoading",                   "DON Areal Loading",                  "continuous", "grey",        "sum"},
        {"biochemistry",  "biochem_don_point_loading",                         "Biochem-DON-PointLoading",                   "DON Point Loading",                  "continuous", "grey",        "sum"},
        {"biochemistry",  "biochem_n_litterfall",                              "Biochem-N-LitterFall",                       "N LitterFall",                       "continuous", "grey",        "sum"},
        {"biochemistry",  "biochem_nh4_areal_loading",                         "Biochem-NH4-ArealLoading",                   "NH4 Areal Loading",                  "continuous", "grey",        "sum"},
        {"biochemistry",  "biochem_nh4_point_loading",                         "Biochem-Nh4-PointLoading",                   "NH4 Point Loading",                  "continuous", "grey",        "sum"},
        {"biochemistry",  "biochem_no3_areal_loading",                         "Biochem-NO3-ArealLoading",                   "NO3 Areal Loading",                  "continuous", "grey",        "sum"},
        {"biochemistry",  "biochem_no3_point_loading",                         "Biochem-NO3-PointLoading",                   "NO3 Point Loading",                  "continuous", "grey",        "sum"},
        {"crop",          "crop_growing_season1",                              "Crop-GrowingSeason1",                        "Growing Season 1",                   "continuous", "grey",        "avg"},
        {"crop",          "crop_growing_season2",                              "Crop-GrowingSeason2",                        "Growing Season 2",                   "continuous", "grey",        "avg"},
        {"crop",          "crop_intensity",                                    "Crop-Intensity",                             "Cropping Intensity",                 "continuous", "grey",        "avg"},
        {"crop",          "crop_irrigation_efficiency",                        "Crop-Irrigation-Efficiency",                 "Irrigation Efficiency",              "continuous", "grey",        "avg"},
        {"crop",          "crop_irrigation_gross_demand",                      "Crop-Irrigation-GrossDemand",                "Irrigation Gross Demand",            "continuous", "grey",        "sum"},
        {"crop",          "crop_irrigation_rice_percolation_rate",             "Crop-Irrigation-RicePercolationRate",        "Rice Percolation Rate",              "continuous", "grey",        "avg"},
        {"hydrology",     "hydrology_discharge",                               "Hydrology-Discharge",                        "Discharge",                          "continuous", "blue",        "avg"},
        {"hydrology",     "hydrology_evapotranspiration",                      "Hydrology-Evapotranspiration",               "Evapotranspiration",                 "continuous", "grey",        "sum"},
        {"hydrology",     "hydrology_evapotranspiration_potential",            "Hydrology-Evapotranspiration-Potential",     "Potential Evapotranspiration",       "continuous", "grey",        "sum"},
        {"hydrology",     "hydrology_evapotranspiration_potential_land",       "Hydrology-Evapotranspiration-PotentialLand", "Potenital Evapotranspiration - Land","continuous", "blue",        "sum"},
        {"hydrology",     "hydrology_evapotranspiration_potential_crop",       "Hydrology-Evapotranspiration-PotentialCrop", "Potential Evapotranspiration - Crop","continuous", "blue",        "sum"},
        {"hydrology",     "hydrology_precipitation",                           "Hydrology-Precipitation",                    "Precipitation",                      "continuous", "blue",        "sum"},
        {"hydrology",     "hydrology_precipitation_daily_fraction",            "Hydrology-Precipitation-DailyFraction",      "Daily Precipitation Fraction",       "continuous", "grey",        "avg"},
        {"hydrology",     "hydrology_precipitation_rainfall",                  "Hydrology-Precipitation-Rainfall",           "Precipitation",                      "continuous", "blue",        "sum"},
        {"hydrology",     "hydrology_precipitation_snowfall",                  "Hydrology-Precipitation-Snowfall",           "Snowfall",                           "continuous", "blue",        "sum"},
        {"hydrology",     "hydrology_runoff",                                  "Hydrology-Runoff",                           "Runoff",                             "continuous", "blue",        "avg"},
        {"hydrology",     "hydrology_runoff_surface",                          "Hydrology-Runoff-Surface",                   "Surface Runoff",                     "continuous", "blue",        "avg"},
        {"hydrology",     "hydrology_runoff_subsurface",                       "Hydrology-Runoff-Subsurface",                "Subsurface Runoff",                  "continuous", "blue",        "avg"},
        {"hydrology",     "hydrology_soil_moisture",                           "Hydrology-SoilMoisture",                     "Soil Moisture",                      "continuous", "grey",        "avg"},
        {"hydrology",     "hydrology_soil_moisture_change",                    "Hydrology-SoilMoisture-Change",              "Soil Moisture Change",               "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_crop",                                "LandFraction_Crop",                          "Crop Area Fraction",                 "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_crop_irrigated",                      "LandFraction_CropIrrigated",                 "Irrigated Area Fraction",            "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_crop_irrigated_other",                "LandFraction-CropIrrigated-Other",           "Irrigated Other Area Fraction",      "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_crop_irrigated_perennial",            "LandFraction-CropIrrigated-Perennial",       "Irrigated Perennial Area Fraction",  "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_crop_irrigated_rice",                 "LandFraction-CropIrrigated-Rice",            "Irrigated Rice Area Fraction",       "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_crop_irrigated_vegetables",           "LandFraction-CropIrrigated-Vegetables",      "Irrigated Vegetables Area Fraction", "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_crop_rainfed",                        "LandFraction-CropRainfed",                   "Rainfed Crop Area Fraction",         "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_barren",                              "LandFraction-Barren",                        "Barren Area Fraction",               "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_vegetation",                          "LandFraction-Vegetation",                    "Vegetation Area Area Fraction",      "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_vegetation_deciduous_broadleaf",      "LandFraction-Vegetation-BroadleafDeciduous", "Deciduous Broadleaf Area Fraction",  "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_vegetation_evergreen_broadleaf",      "LandFraction-Vegetation-BroadleafEvergreen", "Evergreen Broadleaf Area Fraction",  "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_vegetation_deciduous_conifer",        "LandFraction-Vegetation-ConiferDeciduous",   "Deciduous Conifer Area Fraction",    "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_vegetation_evergreen_conifer",        "LandFraction-Vegetation-ConiferEvergreen",   "Evergreen Conifer Area Fraction",    "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_vegetation_grassland",                "LandFraction-Vegetation-Grassland",          "Grassland Area Fraction",            "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_vegetation_mixed_forest",             "LandFraction-Vegetation-MixedForest",        "Mixed Forest Area Fraction",         "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_vegetation_mixed_savannah",           "LandFraction-Vegetation-Savannah",           "Savannah Area Fraction",             "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_vegetation_mixed_woody_savannah",     "LandFraction-Vegetation-SavannahWoody",      "Woody Savannah Area Fraction",       "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_vegetation_mixed_closed_shrubland",   "LandFraction-Vegetation-ShrublandClosed",    "Closed Shrubland Area Fraction",     "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_vegetation_mixed_open_shrubland",     "LandFraction-Vegetation-ShrublandOpen",      "Open Shrubland Area Fraction",       "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_snow_ice",                            "LandFraction-SnowIce",                       "Snow-Ice Area Fraction",             "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_urban_built-up",                      "LandFraction-UrbanBuiltUp",                  "Urban Built-Up Area Fraction",       "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_water",                               "LandFraction-Water",                         "Water Area Fraction",                "continuous", "grey",        "avg"},
        {"land_fraction", "land_fraction_wetland",                             "LandFraction-Wetland",                       "Wetland Area Fraction",              "continuous", "grey",        "avg"},
        {"river",         "river_basin",                                       "River-Basin",                                "River Basin",                        "discrete",   "N/A",         "N/A"},
        {"river",         "river_mouth",                                       "River-Mouth",                                "River Mouth",                        "point",      "N/A",         "N/A"},
        {"river",         "river_cell_area",                                   "River-CellArea",                             "River Cell Area",                    "continuous", "grey",        "max"},
        {"river",         "river_cell_slope",                                  "River-CellSlope",                            "River Cell Slope",                   "continuous", "grey",        "avg"},
        {"river",         "river_confluence",                                  "River-Confluence",                           "River Confluence",                   "point",      "N/A",         "N/A"},
        {"river",         "river_corridor",                                    "River-Corridor",                             "River Corridor",                     "line",       "N/A",         "N/A"},
        {"river",         "river_distance_to_ocean",                           "River-DistToOcean",                          "Distance to Ocean",                  "continuous", "grey",        "max"},
        {"river",         "river_network",                                     "River-Network",                              "STNetwork",                          "network",    "N/A",         "N/A"},
        {"river",         "river_ocean_basin",                                 "River-OceanBasin",                           "Ocean Basin",                        "discrete",   "N/A",         "N/A"},
        {"river",         "river_streamline",                                  "River-Streamline",                           "Streamline",                         "line",       "N/A",         "N/A"},
        {"river",         "river_subbasin",                                    "River-Subbasin",                             "Subbasin",                           "discrete",   "N/A",         "N/A"},
        {"river",         "river_travel_time",                                 "River-TravelTime",                           "Travel Time",                        "continuous", "grey",        "max"},
        {"river",         "river_upstream_area",                               "River-UpstreamArea",                         "Upstream Area",                      "continuous", "grey",        "avg"},
        {"radiation",     "radiation_longwave_downwelling",                    "Radiation-LongWave-Downwelling",             "Downwelling Long Wave Radiation",    "continuous", "grey",        "avg"},
        {"radiation",     "radiation_longwave_upwelling",                      "Radiation-LongWave-Upwelling",               "Upwelling Long Wave Radiation",      "continuous", "grey",        "avg"},
        {"radiation",     "radiation_shortwave_clear_sky",                     "Radiation-ShortWave-ClearSky",               "Clear Sky Short Wave Radiation",     "continuous", "grey",        "avg"},
        {"radiation",     "radiation_shortwave_downwelling",                   "Radiation-ShortWave-Downwelling",            "Downwelling Short Wave Radiation",   "continuous", "grey",        "avg"},
        {"radiation",     "radiation_shortwave_upwelling",                     "Radiation-ShortWave-Upwelling",              "Upwelling Short Wave Radiation",     "continuous", "grey",        "avg"},
        {"reservoir",     "reservoir",                                         "Reservoir",                                  "Reservoir",                          "point",      "N/A",         "N/A"},
        {"reservoir",     "reservoir_area",                                    "Reservoir-Area",                             "Reservoir Area",                     "continuous", "grey",        "sum"},
        {"reservoir",     "reservoir_capacity",                                "Reservoir-Capacity",                         "Reservoir Capacity",                 "continuous", "grey",        "sum"},
        {"reservoir",     "reservoir_height",                                  "Reservoir-Height",                           "Reservoir Height",                   "continuous", "grey",        "avg"},
        {"reservoir",     "reservoir_volume",                                  "Reservoir-Volume",                           "Reservoir Volume",                   "continuous", "grey",        "sum"},
        {"reservoir",     "reservoir_small_coefficient",                       "Reservoir-Small-Coefficient",                "Small-reservoir Coefficient",        "continuous", "grey",        "avg"},
        {"socioeconomy",  "socioeconomy_energy_regions",                       "Socioeconomy-Energy-Regions",                "Energy Regions",                     "discrete",   "N/A",         "N/A"},
        {"socioeconomy",  "socioeconomy_population",                           "Socioeconomy-Population",                    "Population",                         "continuous", "grey",        "sum"},
        {"socioeconomy",  "socioeconomy_population_density",	                  "Socioeconomy-Population-Density",            "Population Density",                 "continuous", "grey",        "avg"},
        {"socioeconomy",  "socioeconomy_population-rural",                     "Socioeconomy-Population-Rural",              "Rural Population",                   "continuous", "grey",        "avg"},
        {"socioeconomy",  "socioeconomy_population-urban",                     "Socioeconomy-Population-Urban",              "Urban Population",                   "continuous", "grey",        "avg"},
        {"soil",          "soil_available_water_capacity",                     "Soil-AvailableWaterCapacity",                "Avaible Water Capacity",             "continuous", "grey",        "avg"},
        {"soil",          "soil_bulk_density",                                 "Soil-BulkDensity",                           "Bulk Density",                       "continuous", "grey",        "avg"},
        {"soil",          "soil_depth_to_horizon",                             "Soil-DepthToRHorizon",                       "Depth to R Horizon",                 "continuous", "grey",        "avg"},
        {"soil",          "soil_depth_to_bedrock",                             "Soil-DepthToBedrock",                        "Depth to Bedrock",                   "continuous", "grey",        "avg"},
        {"soil",          "soil_field_capacity",                               "Soil-FieldCapacity",                         "Field Capacity",                     "continuous", "grey",        "avg"},
        {"soil",          "soil_clay_percent_by_volume",                       "Soil-ClayPercentByVolume",                   "Percent Clay by Volume",             "continuous", "grey",        "avg"},
        {"soil",          "soil_clay_percent_by_mass",                         "Soil-ClayPercentByMass",                     "Percent Clay by Mass",               "continuous", "grey",        "avg"},
        {"soil",          "soil_coarse_percent_by_volume",                     "Soil-CoarsePercentByVolume",                 "Percent Coarse by Volume",           "continuous", "grey",        "avg"},
        {"soil",          "soil_coarse_percent_by_mass",                       "Soil-CoarsePercentByMass",                   "Percent Coarse by Mass",             "continuous", "grey",        "avg"},
        {"soil",          "soil_particle_density",                             "Soil-ParticleDensity",                       "Particle Density",                   "continuous", "grey",        "avg"},
        {"soil",          "soil_porosity",                                     "Soil-Porosity",                              "Porosity",                           "continuous", "grey",        "avg"},
        {"soil",          "soil_probability_of_horizon",                       "Soil-ProbabilityOfRHorizon",                 "Probability of R Horizon",           "continuous", "grey",        "avg"},
        {"soil",          "soil_rooting_depth",                                "Soil-RootingDepth",                          "Rooting Depth",                      "continuous", "grey",        "sum"},
        {"soil",          "soil_sand_percent_by_volume",                       "Soil-SandPercentByVolume",                   "Percent Sand by Volume",             "continuous", "grey",        "avg"},
        {"soil",          "soil_sand_percent_by_mass",                         "Soil-SandPercentByMass",                     "Percent Sand by Mass",               "continuous", "grey",        "avg"},
        {"soil",          "soil_silt_percent_by_volume",                       "Soil-SiltPercentByVolume",                   "Percent Silt by Volume",             "continuous", "grey",        "avg"},
        {"soil",          "soil_silt_percent_by_mass",                         "Soil-SiltPercentByMass",                     "Percent Silt by Mass",               "continuous", "grey",        "avg"},
        {"soil",          "soil_wilting_point",                                "Soil-WiltingPoint",                          "Wilting Point",                      "continuous", "grey",        "avg"},
        {"geography",     "geography_continent",                               "Geography-Continent",                        "Continent",                          "discrete",   "N/A",         "N/A"},
        {"geography",     "geography_county",                                  "Geography-County",                           "County",                             "discrete",   "N/A",         "N/A"},
        {"geography",     "geography_county_crossing",                         "Geography-CountyCrossing",                   "County Crossing",                    "point",      "N/A",         "N/A"},
        {"geography",     "geography_country",                                 "Geography-Country",                          "Country",                            "discrete",   "N/A",         "N/A"},
        {"geography",     "geography_country_crossing",                        "Geography-CountryCrossing",                  "Country Crossing",                   "point",      "N/A",         "N/A"},
        {"geography",     "geography_state",                                   "Geography-State",                            "State",                              "discrete",   "N/A",         "N/A"},
        {"geography",     "geography_state_crossing",                          "Geography-StateCrossing",                    "State Crossing",                     "point",      "N/A",         "N/A"},
        {"geography",     "geography_landmask",                                "Geography-Landmask",                         "Landmask",                           "continuous", "grey",        "avg"},
        {"geography",     "geography_hydrological_unit",                       "Geography-HydrologicalUnit",                 "Hydrological Unit",                  "discrete",   "N/A",         "N/A"},
        {"geography",     "geography_hydrological_unit_crossing",              "Geography-HydrologicalUnitCrossing",         "Hydrological Unit Crossing",         "point",      "N/A",         "N/A"},
        {"station",       "station_discharge",                                 "Station-Discharge",                          "Discharge Gauge",                    "point",      "N/A",         "N/A"},
        {"station",       "station_precipitation",                             "Station-Precipitation",                      "Precipitation Gauge",                "point",      "N/A",         "N/A"},
        {"station",       "station_power_plant",                               "Station-PowerPlant",                         "Power Plant Catchment",              "point",      "N/A",         "N/A"},
        {"topography",    "topography_bathymetry",                             "Topography-Bathymetry",                      "Bathymetry",                         "continuous", "blue",        "avg"},
        {"topography",    "topography_bathymetry-min",                         "Topography-Bathymetry-Min",                  "Minimum Depth",                      "continuous", "blue",        "avg"},
        {"topography",    "topography_bathymetry-max",                         "Topography-Bathymetry-Max",                  "Maximum Depth",                      "continuous", "blue",        "avg"},
        {"topography",    "topography_elevation",                              "Topography-Elevation",                       "Elevation",                          "continuous", "elevation",   "avg"},
        {"topography",    "topography_elevation-min",                          "Topography-Elevation-Min",                   "Minimum Elevation",                  "continuous", "elevation",   "avg"},
        {"topography",    "topograpgy-elevation-max",                          "Topography-Elevation-Max",                   "Maximum Elevation",                  "continuous", "elevation",   "avg"},
        {"weather",       "weather_air_temperature",                           "Weather-AirTemperature",                     "Air Temperature",                    "continuous", "blue-to-red", "avg"},
        {"weather",       "weather_air_temperature-min",                       "Weather-AirTemperature-Min",                 "Minimum Air Temperature",            "continuous", "blue-to-red", "avg"},
        {"weather",       "weather_air_temperature-max",                       "Weather-AirTemperature-Max",                 "Maximum Air Temperature",            "continuous", "blue-to-red", "avg"},
        {"weather",       "weather_air_temperature_diurnal_range",             "Weather-AirTemperature-Range",               "Diurnal Air Temperature Range",      "continuous", "grey",        "avg"},
        {"weather",       "weather_cloud_cover",                               "Weather-CloudCover",                         "Cloud Cover",                        "continuous", "grey",        "avg"},
        {"weather",       "weather_heatindex",                                 "Weather-Heatindex",                          "Heatindex",                          "continuous", "blue-to-red", "avg"},
        {"weather",       "weather_heatindex-max",                             "Weather-Heatindex-Max",                      "Maximum Heatindex",                  "continuous", "blue-to-red", "avg"},
        {"weather",       "weather_heatindex-min",                             "Weather-Heatindex-Min",                      "Minimum Heatindex",                  "continuous", "blue-to-red", "avg"},
        {"weather",       "weather_humidity_dewpoint",                         "Weather-Humidity-Dewpoint",                  "Dewpoint Temperature",               "continuous", "bllue-to-red","avg"},
        {"weather",       "weather_humidity_relative",                         "Weather-Humidity-Relative",                  "Relative Humidity",                  "continuous", "blue",        "avg"},
        {"weather",       "weather_humidity_specific",                         "Weather-Humidity-Specific",                  "Specific Humidity",                  "continuous", "blue",        "avg"},
        {"weather",       "weather_humidity_vapor_pressure",                   "Weather-Humidity-VaporPressure",             "Vapor Pressure",                     "continuous", "blue",        "avg"},
        {"weather",       "weather_air_pressure",                              "Weather-AirPressure",                        "Air Pressure",                       "continuous", "grey",        "avg"},
        {"weather",       "weather_wind_speed",                                "Weather-WindSpeed",                          "Wind Speed",                         "continuous", "grey",        "avg"},
        {"weather",       "weather_wind_speed_u10m",                           "Weather-WindSpeed-U10m",                     "Wind Speed U-direction at 10m",      "continuous", "grey",        "avg"},
        {"weather",       "weather_wind_speed_v10m",                           "Weather-WindSpeed-V10m",                     "Wind Speed V-direction at 10m",      "continuous", "grey",        "avg"}};

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
            for (i = 0; i < VDBrecordNum2; ++i) {
                if (strcmp (cfName, _VDBmasterTable2[i].CFname) == 0) return (_VDBmasterTable2[i].DataType);
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