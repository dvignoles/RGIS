/******************************************************************************

GHAAS Command Line Library V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDBvariableDB.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <string.h>
#include <vdb.h>

static VDBmasterTable_t _VDBmasterTable [] = {
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
        {"hydrology",     "hydrology_evapotranspiration_potential_land",       "Hydrology-Evapotranspiration-Potential-Land","Potenital Evapotranspiration - Land","continuous", "blue",        "sum"},
        {"hydrology",     "hydrology_evapotranspiration_potential_crop",       "Hydrology-Evapotranspiration-Potential-Crop","Potential Evapotranspiration - Crop","continuous", "blue",        "sum"},
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
        {"social",        "social_population",                                 "Social_Population",                          "Population",                         "continuous", "grey",        "sum"},
        {"social",        "social_population_density",	                      "Social_Population-Density",                  "Population Density",                 "continuous", "grey",        "avg"},
        {"socio",         "social_population-rural",                           "Social_Population-Rural",                    "Rural Population",                   "continuous", "grey",        "avg"},
        {"socio",         "social_population-urban",                           "Social_Population-Urban",                    "Urban Population",                   "continuous", "grey",        "avg"},
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
        {"topography",    "topo-bathymetry",                                   "Topo-Bathymetry",                            "Bathymetry",                         "continuous", "blue",        "avg"},
        {"topography",    "topo-bathymetry_min",                               "Topo-Bathymetry-Min",                        "Minimum Depth",                      "continuous", "blue",        "avg"},
        {"topography",    "topo-bathymetry_max",                               "Topo-Bathymetry-Max",                        "Maximum Depth",                      "continuous", "blue",        "avg"},
        {"topography",    "topo-elevation",                                    "Topo-Elevation",                             "Elevation",                          "continuous", "elevation",   "avg"},
        {"topography",    "topo-min_elevation",                                "Topo-Elevation-Min",                         "Minimum Elevation",                  "continuous", "elevation",   "avg"},
        {"topography",    "topo-max_elevation",                                "Topo-Elevation-Max",                         "Maximum Elevation",                  "continuous", "elevation",   "avg"},
        {"weather",       "weather_air_temperature",                           "Weather-AirTemperature",                     "Air Temperature",                    "continuous", "blue-to-red", "avg"},
        {"weather",       "weather_air_temperature_min",                       "Weather-AirTemperature-Min",                 "Minimum Air Temperature",            "continuous", "blue-to-red", "avg"},
        {"weather",       "weather_air_temperature_max",                       "Weather-AirTemperature-Max",                 "Maximum Air Temperature",            "continuous", "blue-to-red", "avg"},
        {"weather",       "weather_air_temperature_diurnal_range",             "Weather-AirTemperature-Range",               "Diurnal AirTemperature Range",       "continuous", "grey",        "avg"},
        {"weather",       "weather_cloud_cover",                               "Weather-CloudCover",                         "Cloud Cover",                        "continuous", "grey",        "avg"},
        {"weather",       "weather_heatindex",                                 "Weather-Heatindex",                          "Heatindex",                          "continuous", "blue-to-red", "avg"},
        {"weather",       "weather_heatindex_max",                             "Weather-Heatindex-Max",                      "Maximum Heatindex",                  "continuous", "blue-to-red", "avg"},
        {"weather",       "weather_heatindex_min",                             "Weather-Heatindex-Min",                      "Minimum Heatindex",                  "continuous", "blue-to-red", "avg"},
        {"weather",       "weather_humidity_dewpoint",                         "Weather-Humidity-Dewpoint",                  "Dewpoint Temperature",               "continuous", "bllue-to-red","avg"},
        {"weather",       "weather_humidity_relative",                         "Weather-Humidity-Relative",                  "Relative Humidity",                  "continuous", "blue",        "avg"},
        {"weather",       "weather_humidity_specific",                         "Weather-Humidity-Specific",                  "Specific Humidity",                  "continuous", "blue",        "avg"},
        {"weather",       "weather_humidity_vapor_pressure",                   "Weather-Humidity-VaporPressure",             "Vapor Pressure",                     "continuous", "blue",        "avg"},
        {"weather",       "weather_air_pressure",                              "Weather-AirPressure",                        "Air Pressure",                       "continuous", "grey",        "avg"},
        {"weather",       "weather_wind_speed",                                "Weather-WindSpeed",                          "Wind Speed",                         "continuous", "grey",        "avg"},
        {"weather",       "weather_wind_speed_u10m",                           "Weather-WindSpeed-U10m",                     "Wind Speed U-direction at 10m",      "continuous", "grey",        "avg"},
        {"weather",       "weather_wind_speed_v10m",                           "Weather-WindSpeed-V10m",                     "Wind Speed V-direction at 10m",      "continuous", "grey",        "avg"}};

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