#!/bin/bash

case "$(uname)" in
    (Linux)
        export GHAASprocessorNum=$(nproc)
    ;;
    (Darwin)
        export GHAASprocessorNum=$(sysctl -n hw.ncpu)
    ;;
    (*)
        export GHAASprocessorNum=1
    ;;
esac

function GHAASprojectDir ()
{
    local arg="${1}"; shift;

    if [[ "${arg%/*}"     != "." ]]; then projectDir="${arg%/*}"; projectDir="${projectDir%/Scripts}"; else prodjectDir=".."; fi
    if [[ "${projectDir}" == "." ]]; then projectDir="$(pwd)"; fi

    echo "${projectDir}"
}

function GHAASscriptName ()
{
    local arg="${1}"; shift;

     scriptName="${arg##*/}"; scriptName="${scriptName%.sh}";
     echo "${scriptName}"
}

export __RGISarchiveFormat="gzipped"

function RGISprocessorNum ()
{
    local processorNum="${0}"; shift

    export GHAASprocessorNum="${processorNum}"
}
function RGISarchiveFormat ()
{
   local format="${1}"

   case "${format}" in
        (plain)
            export __RGISarchiveFormat="plain"
        ;;
        (gzipped)
            export __RGISarchiveFormat="gzipped"
        ;;
        (netcdf)
            export __RGISarchiveFormat="netcdf"
        ;;
        (*)
            echo "Unrecognised file format:${format} in: RGISarchiveFormat"
            export __RGISarchiveFormat="plain"
        ;;
   esac
}

function RGIScase ()
{
    local caseVal="${1}"; shift
    local  string="${1}"; shift

    case "${caseVal}" in
        (lower)
            echo "$(echo "${string}"  | tr "[A-Z]" "[a-z]")"
        ;;
        (upper)
            echo "$(echo "${string}"  | tr "[a-z]" "[A-Z]")"
        ;;
        (*)
            echo "${string}"
        ;;
    esac
}

function RGISlookupSubject ()
{
    local variable="${1}"; shift

	case "$(echo "${variable}" | tr "[A-Z]" "[a-z]")" in
		(air_temperature)                             #   0
			echo "AirTemperature"
		;;
		(min_air_temperature)                         #   1
			echo "AirTemperature-Min"
		;;
		(max_air_temperature)                         #   2
			echo "AirTemperature-Max"
		;;
		(diurnal_air_temperature_range)               #   3
			echo "AirTemperature-Range"
		;;
		(c_litterfall)                                #   4
			echo "C-LitterFall"
		;;
		(cell_area)                                   #   5
			echo "CellArea"
		;;
		(cell_slope)                                  #   6
			echo "CellSlope"
		;;
		(cloud_cover)                                 #   7
			echo "CloudCover"
		;;
		(confluence)                                  #   8
			echo "Confluence"
		;;
		(continents)                                  #   9
			echo "Continents"
		;;
		(countries)                                   #  10
			echo "Countries"
		;;
		(states)                                      #  11
			echo "States"
		;;
		(counties)                                    #  12
			echo "Counties"
		;;
		(cropland_fraction)                           #  13
			echo "Cropland-Fraction"
		;;
		(cropping_intensity)                          #  14
			echo "Crops-Intensity"
		;;
		(daily_precipitation_fraction)                #  15
			echo "Precipitation-DailyFraction"
		;;
		(din_areal_loading)                           #  16
			echo "DIN-AreaLoading"
		;;
		(din_point_loading)                           #  17
			echo "DIN-PointLoading"
		;;
		(discharge)                                   #  18
			echo "Discharge"
		;;
		(distance_to_oceans)                          #  19
			echo "DistToOceans"
		;;
		(doc_areal_loading)                           #  20
			echo "DOC-ArealLoading"
		;;
		(doc_point_loading)                           #  21
			echo "DOC-PointLoading"
		;;
		(don_areal_loading)                           #  22
			echo "DON-ArealLoading"
		;;
		(don_point_loading)                           #  23
			echo "DON-PointLoading"
		;;
		(elevation)                                   #  24
			echo "Elevation"
		;;
		(min_elevation)                               #  25
			echo "Elevation-Min"
		;;
		(max_elevation)                               #  26
			echo "Elevation-Max"
		;;
		(evapotranspiration)                          #  27
			echo "Evapotranspiration"
		;;
		(potential_evapotranspiration)                #  28
			echo "PotEvapotranspiration"
		;;
		(field_capacity)                              #  29
			echo "Soil-FieldCapacity"
		;;
		(growing_season1)                             #  30
			echo "Crops-GrowingSeason1"
		;;
		(growing_season2)                             #  31
			echo "Crops-GrowingSeason2"
		;;
		(heatindex)                                   #  32
			echo "Heatindex"
		;;
		(hydro_unit)                                  #  33
			echo "Hydro-Unit"
		;;
		(max_heatindex)                               #  34
			echo "Heatindex-Max"
		;;
		(min_heatindex)                               #  35
			echo "Heatindex-Min"
		;;
		(dewpoint_temperature)                        #  36
			echo "Humidity-Dewpoint"
		;;
		(relative_humidity)                           #  37
			echo "Humidity-Relative"
		;;
		(specific_humidity)                           #  38
			echo "Humidity-Specific"
		;;
		(vapor_pressure)                              #  39
			echo "Humidity-VaporPressure"
		;;
		(irrigated_area_fraction)                     #  40
			echo "Irrigation-AreaFraction"
		;;
		(irrigation_efficiency)                       #  41
			echo "Irrigation-Efficiency"
		;;
		(irrigation_gross_demand)                     #  42
			echo "IrrGrossDemand"
		;;
		(landcover_barren_percentage)                 #  43
			echo "Landcover-PctBarren"
		;;
		(landcover_deciduous_broadleaf_percentage)    #  44
			echo "Landcover-PctBroadleafDeciduous"
		;;
		(landcover_evergreen_broadleaf_percentage)    #  45
			echo "Landcover-PctBroadleafEvergreen"
		;;
		(landcover_deciduous_conifer_percentage)      #  46
			echo "Landcover-PctConiferDeciduous"
		;;
		(landcover_evergreen_conifer_percentage)      #  47
			echo "Landcover-PctConiferEvergreen"
		;;
		(landcover_cropland_percentage)               #  48
			echo "Landcover-PctCropland"
		;;
		(landcover_crop-natural_mosaic_percentage)    #  49
			echo "Landcover-PctCropNaturalMosaic"
		;;
		(landcover_grassland_percentage)              #  50
			echo "Landcover-PctGrassland"
		;;
		(landcover_mixed_forest_percentage)           #  51
			echo "Landcover-PctMixedForest"
		;;
		(landcover_mixed_savannah_percentage)         #  52
			echo "Landcover-PctSavannah"
		;;
		(landcover_mixed_woody_savannah_percentage)   #  53
			echo "Landcover-PctSavannahWoody"
		;;
		(landcover_mixed_closed_shrubland_percentage) #  54
			echo "Landcover-PctShrublandClosed"
		;;
		(landcover_mixed_open_shrubland_percentage)   #  55
			echo "Landcover-PctShrublandOpen"
		;;
		(landcover_mixed_snow_ice_percentage)         #  56
			echo "Landcover-PctSnowIce"
		;;
		(landcover_urban_built-up_percentage)         #  57
			echo "Landcover-PctUrbanBuiltUp"
		;;
		(landcover_vegetation_percentage)             #  58
			echo "Landcover-PctVegetation"
		;;
		(landcover_water_percentage)                  #  59
			echo "Landcover-PctWater"
		;;
		(landcover_wetland_percentage)                #  60
			echo "Landcover-PctWetland"
		;;
		(network)                                     #  61
			echo "Network"
		;;
		(n_litterfall)                                #  62
			echo "N-LitterFall"
		;;
		(nh4_areal_loading)                           #  63
			echo "NH4-ArealLoading"
		;;
		(nh4_point_loading)                           #  64
			echo "Nh4-PointLoading"
		;;
		(no3_areal_loading)                           #  65
			echo "NO3-ArealLoading"
		;;
		(no3_point_loading)                           #  66
			echo "NO3-PointLoading"
		;;
		(ocean_basins)                                #  67
			echo "OceanBasins"
		;;
		(other_crop_fraction)                         #  68
			echo "CropFraction-Other"
		;;
		(perennial_crop_fraction)                     #  69
			echo "CropFraction-Perennial"
		;;
		(population)                                  #  70
			echo "Population"
		;;
		(population_density)	                      #  71
			echo "Population-Density"
		;;
		(population-rural)                            #  72
			echo "Population-Rural"
		;;
		(population-urban)                            #  73
			echo "Population-Urban"
		;;
		(precipitation)                               #  74
			echo "Precipitation"
		;;
		(shortwave_downwelling_radiation)             #  75
			echo "Radiation-ShortWave-Downwelling"
		;;
		(shortwave_upwelling_radiation)               #  76
			echo "Radiation-ShortWave-Upwelling"
		;;
		(shortwave_clear_sky_radiation)               #  77
			echo "Radiation-ShortWave-ClearSky"
		;;
		(longwave_downwelling_radiation)              #  78
			echo "Radiation-LongWave-Downwelling"
		;;
		(longwave_upwelling_radiation)                #  79
			echo "Radiation-LongWave-Upwelling"
		;;
		(longwave_clear_sky_radiation)                #  80
			echo "Radiation-LongWave-ClearSky"
		;;
		(rain_fall)                                   #  81
			echo "RainFall"
		;;
		(rain_pet)                                    #  82
			echo "RainPET"
		;;
		(reservoir_capacity)                          #  83
			echo "Reservoir-Capacity"
		;;
		(rice_crop_fraction)                          #  84
			echo "CropFraction-Rice"
		;;
		(rice_percolation_rate)                       #  85
			echo "Crops-RicePercolationRate"
		;;
		(rooting_depth)                               #  86
			echo "Soil-RootingDepth"
		;;
		(runoff)                                      #  87
			echo "Runoff"
		;;
		(small_reservoir_coefficient)                 #  88
			echo "SmallReservoir-Coefficient"
		;;
		(snow_fall)                                   #  89
			echo "SnowFall"
		;;
		(soil_moisture)                               #  90
			echo "SoilMoisture"
		;;
		(soil_moisture_change)                        #  91
			echo "SoilMoistureChange"
		;;
		(streamline)                                  #  92
			echo "Streamline"
		;;
		(subbasin)                                    #  93
			echo "Subbasin"
		;;
		(surface_air_pressure)                        #  94
			echo "AirPressure"
		;;
		(surface_air_pressure)                        #  94
			echo "AirPressure"
		;;
		(travel_time)                                 #  95
			echo "TravelTime"
		;;
		(upstream_area)                               #  96
			echo "UpstreamArea"
		;;
		(vegetables_crop_fraction)                    #  97
			echo "CropFraction-Vegetables"
		;;
		(wilting_point)                               #  98
			echo "Soil-WiltingPoint"
		;;
		(wind_speed)                                  #  99
			echo "WindSpeed"
		;;
		(u_wind_speed)                                # 100
			echo "WindSpeed-U10m"
		;;
		(v_wind_speed)                                # 101
			echo "WindSpeed-V10m"
		;;
		(*)
			echo "${variable}"
		;;
	esac
}

function _RGISlookupFullName ()
{
    local variable="${1}"; shift

	case "$(echo "${variable}" | tr "[A-Z]" "[a-z]")" in
		(air_temperature)                             #   0
			echo "Air Temperature"
		;;
		(max_air_temperature)                         #   1
			echo "Maximum Air Temperature"
		;;
		(min_air_temperature)                         #   2
			echo "Minimum Air Temperature"
		;;
		(diurnal_air_temperature_range)               #   3
			echo "Diurnal AirTemperature Range"
		;;
		(c_litterfall)                                #   4
			echo "C LitterFall"
		;;
		(cell_area)                                   #   5
			echo "Cell Area"
		;;
		(cell_slope)                                  #   6
			echo "Cell Slope"
		;;
		(cloud_cover)                                 #   7
			echo "Cloud Cover"
		;;
		(confluence)                                  #   8
			echo "Confluence"
		;;
		(continents)                                  #   9
			echo "Continents"
		;;
		(countries)                                   #  10
			echo "Countries"
		;;
		(states)                                      #  11
			echo "States"
		;;
		(counties)                                    #  12
			echo "Counties"
		;;
		(cropland_fraction)                           #  13
			echo "Cropland Fraction"
		;;
		(cropping_intensity)                          #  14
			echo "Cropping Intensity"
		;;
		(daily_precipitation_fraction)                #  15
			echo "Daily Precipitation Fraction"
		;;
		(din_areal_loading)                           #  16
			echo "DIN Areal Loading"
		;;
		(din_point_loading)                           #  17
			echo "DIN Point Loading"
		;;
		(discharge)                                   #  18
			echo "Discharge"
		;;
		(distance_to_oceans)                          #  19
			echo "Distance to Oceans"
		;;
		(doc_areal_loading)                           #  20
			echo "DOC Areal Loading"
		;;
		(doc_point_loading)                           #  21
			echo "DOC Point Loading"
		;;
		(don_areal_loading)                           #  22
			echo "DON Areal Loading"
		;;
		(don_point_loading)                           #  23
			echo "DON Point Loading"
		;;
		(elevation)                                   #  24
			echo "Elevation"
		;;
		(max_elevation)                               #  25
			echo "Maximum Elevation"
		;;
		(min_elevation)                               #  26
			echo "Minimum Elevation"
		;;
		(evapotranspiration)                          #  27
			echo "Evapotranspiration"
		;;
		(potential_evapotranspiration)                #  28
			echo "Potential Evapotranspiration"
		;;
		(field_capacity)                              #  29
			echo "Field Capacity"
		;;
		(growing_season1)                             #  30
			echo "Growing Season 1"
		;;
		(growing_season2)                             #  31
			echo "Growing Season 2"
		;;
		(heatindex)                                   #  32
			echo "Heatindex"
		;;
		(hydro_unit)                                  #  33
			echo "Hydrological Unit"
		;;
		(max_heatindex)                               #  34
			echo "Maximum Heatindex"
		;;
		(min_heatindex)                               #  35
			echo "Minimum Heatindex"
		;;
		(dewpoint_temperature)                        #  36
			echo "Dewpoint Temperature"
		;;
		(relative_humidity)                           #  37
			echo "Relative Humidity"
		;;
		(specific_humidity)                           #  38
			echo "Specific Humidity"
		;;
		(vapor_pressure)                              #  39
			echo "Vapor Pressure"
		;;
		(irrigated_area_fraction)                     #  40
			echo "Irrigated Area Fraction"
		;;
		(irrigation_efficiency)                       #  41
			echo "Irrigation Efficiency"
		;;
		(irrigation_gross_demand)                     #  42
			echo "Irrigation Gross Demand"
		;;
		(landcover_barren_percentage)                 #  43
			echo "Barren Percentage"
		;;
		(landcover_deciduous_broadleaf_percentage)    #  44
			echo "Deciduous Broadleaf Percentage"
		;;
		(landcover_evergreen_broadleaf_percentage)    #  45
			echo "Evergreen Broadleaf Percentage"
		;;
		(landcover_deciduous_conifer_percentage)      #  46
			echo "Deciduous Conifer Percentage"
		;;
		(landcover_evergreen_conifer_percentage)      #  47
			echo "Evergreen Conifer Percentage"
		;;
		(landcover_cropland_percentage)               #  48
			echo "Cropland Percentage"
		;;
		(landcover_crop-natural_mosaic_percentage)    #  49
			echo "Cropland-Natural Mosaic Percentage"
		;;
		(landcover_grassland_percentage)              #  50
			echo "Grassland Percentage"
		;;
		(landcover_mixed_forest_percentage)           #  51
			echo "Mixed Forest Percentage"
		;;
		(landcover_mixed_savannah_percentage)         #  52
			echo "Savannah Percentage"
		;;
		(landcover_mixed_woody_savannah_percentage)   #  53
			echo "Woody Savannah Percentage"
		;;
		(landcover_mixed_closed_shrubland_percentage) #  54
			echo "Closed Shrubland Percentage"
		;;
		(landcover_mixed_open_shrubland_percentage)   #  55
			echo "Open Shrubland Percentage"
		;;
		(landcover_mixed_snow_ice_percentage)         #  56
			echo "Snow-Ice Percentage"
		;;
		(landcover_urban_built-up_percentage)         #  57
			echo "Urban Built-Up Percentage"
		;;
		(landcover_vegetation_percentage)             #  58
			echo "Vegetation Fraction"
		;;
		(landcover_water_percentage)                  #  59
			echo "Water Percentage"
		;;
		(landcover_wetland_percentage)                #  60
			echo "Wetland Percentage"
		;;
		(network)                                     #  61
			echo "STNetwork"
		;;
		(n_litterfall)                                #  62
			echo "N LitterFall"
		;;
		(nh4_areal_loading)                           #  63
			echo "NH4 Areal Loading"
		;;
		(nh4_point_loading)                           #  64
			echo "NH4 Point Loading"
		;;
		(no3_areal_loading)                           #  65
			echo "NO3 Areal Loading"
		;;
		(no3_point_loading)                           #  66
			echo "NO3 Point Loading"
		;;
		(ocean_basins)                                #  67
			echo "Ocean Basins"
		;;
		(other_crop_fraction)                         #  68
			echo "Other Crop Fraction"
		;;
		(perennial_crop_fraction)                     #  69
			echo "Perennial Crop Fraction"
		;;
		(population)                                  #  70
			echo "Population"
		;;
		(population_density)                          #  71
			echo "Population Density"
		;;
		(population-rural)                            #  72
			echo "Rural Population"
		;;
		(population-urban)                            #  73
			echo "Urban Population"
		;;
		(precipitation)                               #  74
			echo "Precipitation"
		;;
		(shortwave_downwelling_radiation)             #  75
 			echo "Downwelling Short Wave Radiation"
		;;
		(shortwave_upwelling_radiation)               #  76
			echo "Upwelling Short Wave Radiation"
		;;
		(shortwave_clear_sky_radiation)               #  77
			echo "Clear Sky Short Wave Radiation"
		;;
		(longwave_downwelling_radiation)              #  78
			echo "Downwelling Long Wave Radiation"
		;;
		(longwave_upwelling_radiation)                #  79
			echo "Upwelling Long Wave Radiation"
		;;
		(longwave_clear_sky_radiation)                #  80
			echo "Clear Sky Long Wave Radiation"
		;;
		(rain_fall)                                   #  81
			echo "Rain Fall"
		;;
		(rain_pet)                                    #  82
			echo "Rain Potential ET"
		;;
		(reservoir_capacity)                          #  83
			echo "Reservoir Capacity"
		;;
		(rice_crop_fraction)                          #  84
			echo "Rice Crop Fraction"
		;;
		(rice_percolation_rate)                       #  85
			echo "Rice Percolation Rate"
		;;
		(rooting_depth)                               #  86
			echo "Rooting Depth"
		;;
		(runoff)                                      #  87
			echo "Runoff"
		;;
		(small_reservoir_coefficient)                 #  88
			echo "Small-reservoir Coefficient"
		;;
		(snow_fall)                                   #  89
			echo "Snow Fall"
		;;
		(soil_moisture)                               #  90
			echo "Soil Moisture"
		;;
		(soil_moisture_change)                        #  91
			echo "Soil Moisture Change"
		;;
		(streamline)                                  #  92
			echo "Streamline"
		;;
		(subbasin)                                    #  93
			echo "Subbasin"
 		;;
		(surface_air_pressure)                        #  94
			echo "Air Pressure"
		;;
		(travel_time)                                 #  95
			echo "Travel Time"
		;;
		(upstream_area)                               #  96
			echo "Upstream Area"
		;;
		(vegetables_crop_fraction)                    #  97
			echo "Vegetables Crop Fraction"
		;;
		(wilting_point)                               #  98
			echo "Wilting Point"
		;;
		(wind_speed)                                  #  99
			echo "Wind Speed"
		;;
		(u_wind_speed)                                # 100
			echo "Wind Speed U-direction"
		;;
		(v_wind_speed)                                # 101
			echo "Wind Speed V-direction"
		;;
		(*)
			echo "${variable}"
		;;
	esac
}

function RGISlookupShadeset ()
{
    local variable="${1}"; shift

	case "$(echo "${variable}" | tr "[A-Z]" "[a-z]")" in
		(air_temperature|max_air_temperature|min_air_temperature) # 0 1 2
			echo "blue-to-red"
		;;
		(diurnal_air_temperature_range)               #   3
			echo "grey"
		;;
		(c_litterfall)                                #   4
			echo "grey"
		;;
		(cell_area)                                   #   5
			echo "grey"
		;;
		(cell_slope)                                  #   6
			echo "grey"
		;;
		(cloud_cover)                                 #   7
			echo "grey"
		;;
		(confluence)                                  #   8
			echo "grey"
		;;
		(continents)                                  #   9
			echo "grey"
		;;
		(countries)                                   #  10
			echo "grey"
		;;
		(states)                                      #  11
			echo "grey"
		;;
		(counties)                                    #  12
			echo "grey"
		;;
		(cropland_fraction)                           #  13
			echo "grey"
		;;
		(cropping_intensity)                          #  14
			echo "grey"
		;;
		(daily_precipitation_fraction)                #  15
			echo "grey"
		;;
		(din_areal_loading)                           #  16
			echo "grey"
		;;
		(din_point_loading)                           #  17
			echo "grey"
		;;
		(discharge)                                   #  18
			echo "blue"
		;;
		(distance_to_oceans)                          #  19
			echo "grey"
		;;
		(don_areal_loading)                           #  20
			echo "grey"
		;;
		(don_point_loading)                           #  21
			echo "grey"
		;;
		(doc_areal_loading)                           #  22
			echo "grey"
		;;
		(doc_point_loading)                           #  23
			echo "grey"
		;;
		(elevation|max_elevation|min_elevation)       #  24 25 26
			echo "elevation"
		;;
		(evapotranspiration)                          #  27
			echo "grey"
		;;
		(potential_evapotranspiration)                #  28
			echo "grey"
		;;
		(field_capacity)                              #  29
			echo "grey"
		;;
		(growing_season1)                             #  30
			echo "grey"
		;;
		(growing_season2)                             #  31
			echo "grey"
		;;
		(heatindex)                                   #  32
			echo "blue-to-red"
		;;
		(hydro_unit)                                  #  33
			echo "grey"
		;;
		(max_heatindex)                               #  34
			echo "blue-to-red"
		;;
		(min_heatindex)                               #  35
			echo "blue-to-red"
		;;
		(dewpoint_temperature)                        #  36
			echo "blue"
		;;
		(relative_humidity)                           #  37
			echo "blue"
		;;
		(specific_humidity)                           #  38
			echo "blue"
		;;
		(vapor_pressure)                              #  39
			echo "blue"
		;;
		(irrigated_area_fraction)                     #  40
			echo "grey"
		;;
		(irrigation_efficiency)                       #  41
			echo "grey"
		;;
		(irrigation_gross_demand)                     #  42
			echo "grey"
		;;
		(landcover_barren_percentage)                 #  43
			echo "grey"
		;;
		(landcover_deciduous_broadleaf_percentage)    #  44
			echo "grey"
		;;
		(landcover_evergreen_broadleaf_percentage)    #  45
			echo "grey"
		;;
		(landcover_deciduous_conifer_percentage)      #  46
			echo "grey"
		;;
		(landcover_evergreen_conifer_percentage)      #  47
			echo "grey"
		;;
		(landcover_cropland_percentage)               #  48
			echo "grey"
		;;
		(landcover_crop-natural_mosaic_percentage)    #  49
			echo "grey"
		;;
		(landcover_grassland_percentage)              #  50
			echo "grey"
		;;
		(landcover_mixed_forest_percentage)           #  51
			echo "grey"
		;;
		(landcover_mixed_savannah_percentage)         #  52
			echo "grey"
		;;
		(landcover_mixed_woody_savannah_percentage)   #  53
			echo "grey"
		;;
		(landcover_mixed_closed_shrubland_percentage) #  54
			echo "grey"
		;;
		(landcover_mixed_open_shrubland_percentage)   #  55
			echo "grey"
		;;
		(landcover_mixed_snow_ice_percentage)         #  56
			echo "grey"
		;;
		(landcover_urban_built-up_percentage)         #  57
			echo "grey"
		;;
		(landcover_vegetation_percentage)             #  58
			echo "grey"
		;;
		(landcover_water_percentage)                  #  59
			echo "grey"
		;;
		(landcover_wetland_percentage)                #  60
			echo "grey"
		;;
		(network)                                     #  61
			echo "grey"
		;;
		(n_litterfall)                                #  62
			echo "grey"
		;;
		(nh4_areal_loading)                           #  63
			echo "grey"
		;;
		(nh4_point_loading)                           #  64
			echo "grey"
		;;
		(no3_areal_loading)                           #  65
			echo "grey"
		;;
		(no3_point_loading)                           #  66
			echo "grey"
		;;
		(ocean_basins)                                #  67
			echo "grey"
		;;
		(other_crop_fraction)                         #  68
			echo "grey"
		;;
		(perennial_crop_fraction)                     #  69
			echo "grey"
		;;
		(population)                                  #  70
			echo "grey"
		;;
		(population_density)                          #  71
			echo "grey"
		;;
		(population-rural)                            #  72
			echo "grey"
		;;
		(population-urban)                            #  73
			echo "grey"
		;;
		(precipitation)                               #  74
			echo "blue"
		;;
		(shortwave_downwelling_radiation)             #  75
			echo "grey"
		;;
		(shortwave_upwelling_radiation)               #  76
			echo "grey"
		;;
		(shortwave_clear_sky_radiation)               #  77
			echo "grey"
		;;
		(longwave_downwelling_radiation)              #  78
			echo "grey"
		;;
		(longwave_upwelling_radiation)                #  79
			echo "grey"
		;;
		(longwave_clear_sky_radiation)                #  80
			echo "grey"
		;;
		(rain_fall)                                   #  81
			echo "blue"
		;;
		(rain_pet)                                    #  82
			echo "grey"
		;;
		(reservoir_capacity)                          #  83
			echo "grey"
		;;
		(rice_crop_fraction)                          #  84
			echo "grey"
		;;
		(rice_percolation_rate)                       #  85
			echo "grey"
		;;
		(rooting_depth)                               #  86
			echo "grey"
		;;
		(runoff)                                      #  87
			echo "blue"
		;;
		(small_reservoir_coefficient)                 #  88
			echo "grey"
		;;
		(snow_fall)                                   #  89
			echo "grey"
		;;
		(soil_moisture)                               #  90
			echo "grey"
		;;
		(soil_moisture_change)                        #  91
			echo "blue-to-red"
		;;
		(surface_air_pressure)                        #  92
			echo "grey"
		;;
		(streamline)                                  #  93
			echo "grey"
		;;
		(subbasin)                                    #  94
			echo "grey"
		;;
		(travel_time)                                 #  95
			echo "grey"
		;;
		(upstream_area)                               #  96
			echo "grey"
		;;
		(vegetables_crop_fraction)                    #  97
			echo "grey"
		;;
		(wilting_point)                               #  98
			echo "grey"
		;;
		(wind_speed)                                  #  99
			echo "grey"
		;;
		(u_wind_speed)                                # 100
			echo "grey"
		;;
		(v_wind_speed)                                # 101
			echo "grey"
		;;
		(*)
			echo "grey"
		;;
	esac
}

function RGISlookupAggrMethod ()
{
    local variable="${1}"; shift

	case "$(echo "${variable}" | tr "[A-Z]" "[a-z]")" in
		(air_temperature|max_air_temperature|min_air_temperature)  # 0 1 2
			echo "avg"
		;;
		(diurnal_air_temperature_range)               #   3
			echo "avg"
		;;
		(c_litterfall)                                #   4
			echo "sum"
		;;
		(cell_area)                                   #   5
			echo "avg"
		;;
		(cell_slope)                                  #   6
			echo "grey"
		;;
		(cloud_cover)                                 #   7
			echo "avg"
		;;
		(confluence)                                  #   8
			echo "N/A"
		;;
		(continents)                                  #   9
			echo "N/A"
		;;
		(countries)                                   #  10
			echo "N/A"
		;;
		(states)                                      #  11
			echo "N/A"
		;;
		(counties)                                    #  12
			echo "N/A"
		;;
		(cropland_fraction)                           #  13
			echo "avg"
		;;
		(cropping_intensity)                          #  14
			echo "avg"
		;;
		(daily_precipitation_fraction)                #  15
			echo "avg"
		;;
		(din_areal_loading)                           #  16
			echo "sum"
		;;
		(din_point_loading)                           #  17
			echo "sum"
		;;
		(discharge)                                   #  18
			echo "avg"
		;;
		(distance_to_oceans)                          #  19
			echo "max"
		;;
		(don_areal_loading)                           #  20
			echo "sum"
		;;
		(don_point_loading)                           #  21
			echo "sum"
		;;
		(doc_areal_loading)                           #  22
			echo "sum"
		;;
		(doc_point_loading)                           #  23
			echo "sum"
		;;
		(elevation|max_elevation|min_elevation)       #  24 25 26
			echo "avg"
		;;
		(evapotranspiration)                          #  27
			echo "sum"
		;;
		(potential_evapotranspiration)                #  28
			echo "sum"
		;;
		(field_capacity)                              #  29
			echo "avg"
		;;
		(growing_season1)                             #  30
			echo "avg"
		;;
		(growing_season2)                             #  31
			echo "avg"
		;;
		(heatindex)                                   #  32
			echo "avg"
		;;
		(hydro_unit)                                  #  33
			echo "grey"
		;;
		(max_heatindex)                               #  34
			echo "avg"
		;;
		(min_heatindex)                               #  35
			echo "avg"
		;;
		(dewpoint_temperature)                        #  36
			echo "avg"
		;;
		(relative_humidity)                           #  37
			echo "avg"
		;;
		(specific_humidity)                           #  38
			echo "avg"
		;;
		(vapor_pressure)                              #  39
			echo "avg"
		;;
		(irrigated_area_fraction)                     #  40
			echo "avg"
		;;
		(irrigation_efficiency)                       #  41
			echo "avg"
		;;
		(irrigation_gross_demand)                     #  42
			echo "sum"
		;;
		(landcover_barren_percentage)                 #  43
			echo "avg"
		;;
		(landcover_deciduous_broadleaf_percentage)    #  44
			echo "avg"
		;;
		(landcover_evergreen_broadleaf_percentage)    #  45
			echo "avg"
		;;
		(landcover_deciduous_conifer_percentage)      #  46
			echo "avg"
		;;
		(landcover_evergreen_conifer_percentage)      #  47
			echo "avg"
		;;
		(landcover_cropland_percentage)               #  48
			echo "avg"
		;;
		(landcover_crop-natural_mosaic_percentage)    #  49
			echo "avg"
		;;
		(landcover_grassland_percentage)              #  50
			echo "avg"
		;;
		(landcover_mixed_forest_percentage)           #  51
			echo "avg"
		;;
		(landcover_mixed_savannah_percentage)         #  51
			echo "avg"
		;;
		(landcover_mixed_woody_savannah_percentage)   #  53
			echo "avg"
		;;
		(landcover_mixed_closed_shrubland_percentage) #  54
			echo "avg"
		;;
		(landcover_mixed_open_shrubland_percentage)   #  55
			echo "avg"
		;;
		(landcover_mixed_snow_ice_percentage)         #  56
			echo "avg"
		;;
		(landcover_urban_built-up_percentage)         #  57
			echo "avg"
		;;
		(landcover_vegetation_percentage)             #  58
			echo "avg"
		;;
		(landcover_water_percentage)                  #  59
			echo "avg"
		;;
		(landcover_wetland_percentage)                #  60
			echo "avg"
		;;
		(network)                                     #  61
			echo "avg"
		;;
		(n_litterfall)                                #  62
			echo "sum"
		;;
		(nh4_areal_loading)                           #  63
			echo "sum"
		;;
		(nh4_point_loading)                           #  64
			echo "sum"
		;;
		(no3_areal_loading)                           #  65
			echo "sum"
		;;
		(no3_point_loading)                           #  66
			echo "sum"
		;;
		(ocean_basins)                                #  67
			echo "N/A"
		;;
		(other_crop_fraction)                         #  68
			echo "avg"
		;;
		(perennial_crop_fraction)                     #  69
			echo "avg"
		;;
		(population)                                  #  70
			echo "sum"
		;;
		(population_density)                          #  71
			echo "avg"
		;;
		(population-rural)                            #  72
			echo "avg"
		;;
		(population-urban)                            #  73
			echo "avg"
		;;
		(precipitation)                               #  74
			echo "sum"
		;;
		(shortwave_downwelling_radiation)             #  75
			echo "avg"
		;;
		(shortwave_upwelling_radiation)               #  76
			echo "avg"
		;;
		(shortwave_clear_sky_radiation)               #  77
			echo "avg"
		;;
		(longwave_downwelling_radiation)              #  78
			echo "avg"
		;;
		(longwave_upwelling_radiation)                #  79
			echo "avg"
		;;
		(longwave_clear_sky_radiation)                #  80
			echo "avg"
		;;
		(rain_fall)                                   #  81
			echo "sum"
		;;
		(rain_pet)                                    #  82
			echo "sum"
		;;
		(reservoir_capacity)                          #  83
			echo "avg"
		;;
		(rice_crop_fraction)                          #  84
			echo "avg"
		;;
		(rice_percolation_rate)                       #  85
			echo "avg"
		;;
		(rooting_depth)                               #  86
			echo "avg"
		;;
		(runoff)                                      #  87
			echo "sum"
		;;
		(small_reservoir_coefficient)                 #  88
			echo "avg"
		;;
		(snow_fall)                                   #  89
			echo "avg"
		;;
		(soil_moisture)                               #  90
			echo "avg"
		;;
		(soil_moisture_change)                        #  91
			echo "avg"
		;;
		(surface_air_pressure)                        #  92
			echo "avg"
		;;
		(streamline)                                  #  93
			echo "avg"
		;;
		(subbasin)                                    #  94
			echo "avg"
		;;
		(travel_time)                                 #  95
			echo "N/A"
		;;
        (upstream_area)                               #  96
			echo "avg"
		;;
		(vegetables_crop_fraction)                    #  97
			echo "avg"
		;;
		(wilting_point)                               #  98
			echo "avg"
		;;
		(wind_speed)                                  #  99
			echo "avg"
		;;
		(u_wind_speed)                                # 100
			echo "avg"
		;;
		(v_wind_speed)                                # 101
			echo "avg"
		;;
		(*)
			echo "avg"
			echo "Defaulting aggregation method: ${variable}" > /dev/stderr
		;;
	esac
}

function RGISlookupTimeStep ()
{
	local timeStep=$(echo "${1}" | tr "[A-Z]" "[a-z]")

	case "${timeStep}" in
		(daily)
			echo "day"
		;;
		(monthly)
			echo "month"
		;;
		(annual)
			echo "year"
		;;
	esac
}

function RGIStimeNumberOfDays ()
{
	local    year="${1}";        shift
	local   month=$((${1} - 1)); shift
	local century="${year%??}"

	local monthLength[0]=31
	local monthLength[1]=28
	local monthLength[2]=31
	local monthLength[3]=30
	local monthLength[4]=31
	local monthLength[5]=30
	local monthLength[6]=31
	local monthLength[7]=31
	local monthLength[8]=30
	local monthLength[9]=31
	local monthLength[10]=30
	local monthLength[11]=31

	if (( month == 1))
	then
		if [[ "${year}" == "${century}00" ]]
		then
			local leapYear=$(( (${century} & 0xffffe) == ${century} ? 1 : 0))
		else
			local leapYear=$(( (${year}    & 0xffffe) == ${year}    ? 1 : 0))
		fi
	else
		local leapYear=0
	fi
	echo $((${monthLength[${month}]} + ${leapYear}))
}

function _RGISvariableDir ()
{
	local      archive="${1}"; shift
	local       domain="${1}"; shift
	local     variable="${1}"; shift

	local       varDir=$(RGISlookupSubject "${variable}")

	if [ -e "${archive}/${domain%+}/${varDir}" ]
	then
		echo "${domain%+}/${varDir}"
		return 0
	else
		if [ "${domain%+}" == "${domain}" ] # Exact domain
		then
			echo "Missing ${archive}/${domain}/${varDir}" > /dev/stderr
			echo ""
			return 1
		else # Dynamic domain
			local parent="${archive}/${domain%+}/parent"
			if [ -e "${parent}" ]
			then
				_RGISvariableDir "${archive}" "$(cat ${archive}/${domain%+}/parent)+" "${variable}" || return 1
			else
				echo "Missing ${parent}" > /dev/stderr
				echo ""
				return 1
			fi
		fi
	fi
	return 0
}

function RGISgeoResolutionInSecond ()
{
    local resolution="${1}"; shift

   	case "${resolution}" in
	("15sec")
		echo "15"
	;;
	("30sec")
		echo "30"
	;;
	("45sec")
		echo "45"
	;;
	("01min")
		echo "60"
	;;
	("1m30s")
		echo "90"
	;;
	("2m30s")
		echo "150"
	;;
	("03min")
		echo "180"
	;;
	("3m45s")
		echo "225"
	;;
	("05min")
		echo "300"
	;;
	("06min")
		echo "360"
	;;
	("10min")
		echo "600"
	;;
	("15min")
		echo "900"
	;;
	("30min")
		echo "1800"
	;;
	("60min")
		echo "3600"
	;;
	("02deg")
		echo "7200"
	;;
	(*)
		echo "Invalid resolution" > /dev/stderr
		return 1
	;;
	esac
    return 0
}

RGISgeoResolutionMultiplier () # Destination resolution is devided by source resolution. Decimals ar chapped off.
{
    local srcRes="$(RGISgeoResolutionInSecond "${1}")"; shift
    local dstRes="$(RGISgeoResolutionInSecond "${1}")"; shift

    if [[ "${srcRes}" == "" || "${dstRes}" == "" ]]
    then
        echo "Invalid resolutions: ${srcRes}, ${dstRes}" > /dev/stderr
        echo "Usage: Destination resolution is devided by source resolution. Decimals ar chapped off." > /dev/stderr
        return 1
    else
        echo "${dstRes} / ${srcRes}" | bc -l | sed "s:\([0-9]*\).*:\1:"
    fi
    return 0

}
function _RGISresolutionDir ()
{
	local      archive="${1}"; shift
	local       domain="${1}"; shift
	local     variable="${1}"; shift
	local      product="${1}"; shift
	local   resolution="${1}"; shift

	local varDir=$(_RGISvariableDir "${archive}" "${domain}" "${variable}")
	if [ "${varDir}" == "" ]
	then
		return
	else
		if [ -e "${archive}/${varDir}/${product}/${resolution%+}" ]
		then
			echo "${varDir}/${product}/${resolution%+}"
		else
			if [ "${resolution%+}" == "${resolution}" ] # Exact resolution
			then
				echo "Missing ${archive}/${varDir}/${product}/${resolution}" > /dev/stderr
				echo ""
				return 1
			else # Dynamic resolution
				if [ "${resolution%km+}" == "${resolution}" ] # Geographic
				then
					local geogNum=0
					local geogRes[${geogNum}]="15sec"; (( ++geogNum ))
					local geogRes[${geogNum}]="30sec"; (( ++geogNum ))
					local geogRes[${geogNum}]="45sec"; (( ++geogNum ))
					local geogRes[${geogNum}]="01min"; (( ++geogNum ))
					local geogRes[${geogNum}]="1m30s"; (( ++geogNum ))
					local geogRes[${geogNum}]="2m30s"; (( ++geogNum ))
					local geogRes[${geogNum}]="03min"; (( ++geogNum ))
					local geogRes[${geogNum}]="3m45s"; (( ++geogNum ))
					local geogRes[${geogNum}]="05min"; (( ++geogNum ))
					local geogRes[${geogNum}]="06min"; (( ++geogNum ))
					local geogRes[${geogNum}]="7m30s"; (( ++geogNum ))
					local geogRes[${geogNum}]="10min"; (( ++geogNum ))
					local geogRes[${geogNum}]="15min"; (( ++geogNum ))
					local geogRes[${geogNum}]="30min"; (( ++geogNum ))
					local geogRes[${geogNum}]="60min"; (( ++geogNum ))
					local geogRes[${geogNum}]="1d30m"; (( ++geogNum ))
					local geogRes[${geogNum}]="02deg"; (( ++geogNum ))
					local geogRes[${geogNum}]="2d30m"; (( ++geogNum ))

					for (( num = 0; num < ${geogNum}; ++num ))
					do
						if [ "${geogRes[${num}]}" == "${resolution%+}" ]; then break; fi
					done
					if (( num < geogNum))
					then
						_RGISresolutionDir "${archive}" "${domain}" "${variable}"  "${product}" "${geogRes[((${num} + 1))]}+" || return 1
					else
						echo "No coarser resolution ${domain%+} ${variable} ${product} ${resolution%+}" > /dev/stderr
						return 1
					fi
				else # Cartesian
					local cartNum=0
					local cartRes[${cartNum}]="1km";    (( ++cartNum ))
					local cartRes[${cartNum}]="2km";    (( ++cartNum ))
					local cartRes[${cartNum}]="5km";    (( ++cartNum ))
					local cartRes[${cartNum}]="10km";   (( ++cartNum ))
					local cartRes[${cartNum}]="25km";   (( ++cartNum ))
					local cartRes[${cartNum}]="50km";   (( ++cartNum ))

					for (( num = 0; num < ${cartNum}; ++num ))
					do
						if [ "${cartRes[${num}]}" == "${resolution%+}" ]; then break; fi
					done
					if (( num < cartNum))
					then
						_RGISresolutionDir "${archive}" "${domain}" "${variable}"  "${product}" "${cartRes[((${num} + 1))]}+" || return 1
					else
						echo "No coarser resolution ${domain%+} ${variable} ${product} ${resolution%+}" > /dev/stderr
						return 1
					fi
				fi
			fi
		fi
	fi
	return 0
}
function _RGIStStepDir ()
{
	local tStepType="${1}"; shift
	local     tStep="${1}"; shift

	case "${tStepType}" in
		(TS)
			case "${tStep}" in
				(hourly)
					echo "Hourly"
				;;
				(3hourly)
					echo "3Hourly"
				;;
				(6hourly)
					echo "6Hourly"
				;;
				(daily)
					echo "Daily"
				;;
				(monthly)
					echo "Monthly"
				;;
				(annual)
					echo "Annual"
				;;
				(*)
					echo "${tStep}"
				;;
			esac
		;;
		(LT|LTmin|LTmax|LTslope|LTrange|LTstdDev|Stats|static)
			echo "Static"
		;;
	esac
	return 0
}

function RGISdirectoryPath ()
{
	local    archive="${1}"; shift
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local  tStepType="${1}"; shift
	local      tStep=$(echo "${1}" | tr "[A-Z]" "[a-z]"); shift

	local        dir=$(_RGIStStepDir ${tStepType} ${tStep})
	local     varDir=$(RGISlookupSubject "${variable}")

	echo "${archive}/${domain}/${varDir}/${product}/${resolution}/${dir}"
}

function RGISdirectory ()
{
	local    archive="${1}"; shift
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local  tStepType="${1}"; shift
	local      tStep=$(echo "${1}" | tr "[A-Z]" "[a-z]")

	local dir=$(_RGIStStepDir ${tStepType} ${tStep})

	local resDir=$(_RGISresolutionDir "${archive}" "${domain}" "${variable}" "${product}" "${resolution}")
	if [ "${resDir}" == "" ]
	then
		return 1
	else
		echo "${archive}/${resDir}/${dir}"
	fi
	return 0
}

function RGISfileExtension ()
{
	local variable="${1}"; shift

	case "$(echo "${variable}" | tr "[A-Z]" "[a-z]")" in
		(network)
			local extension="gdbn"
		;;
		(confluence|crossing-*)
			local extension="gdbp"
		;;
		(continents|countries|states|counties|ocean_basins|subbasin|hydro_unit)
			local extension="gdbd"
		;;
		(streamline)
			local extension="gdbl"
		;;
		(*)
			local extension="gdbc"
		;;
    esac
    echo "${extension}"
}

function RGISfilePath ()
{
	local      archive="${1}"; shift
	local       domain="${1}"; shift
	local     variable="${1}"; shift
	local      product="${1}"; shift
	local   resolution="${1}"; shift
	local    tStepType="${1}"; shift
	local        tStep=$(echo "${1}" | tr "[A-Z]" "[a-z]"); shift
	local    timeRange="${1}"; shift

	USAGE="Usage: RGISfilePath <archive> <domain> <variable> <product> <resolution> <tstep type> <tstep> [time range]"
    if [[ "${resolution}" == "" ]]; then echo "${USAGE}"; return 1; fi

	case "${tStep}" in
		(hourly)
			local tStepStr="h"
		;;
		(3hourly)
			local tStepStr="3h"
		;;
		(6hourly)
			local tStepStr="6h"
		;;
		(daily)
			local tStepStr="d"
		;;
		(monthly)
			local tStepStr="m"
		;;
		(annual)
			local tStepStr="a"
		;;
		("")
			local tStepStr=""
		;;
		(*)
			echo "Unknown time step ${tStep}" > /dev/stderr
			return 1
		;;
	esac

    local extension=$(RGISfileExtension "${variable}")

	local rgisDirectory=$(RGISdirectoryPath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}")
	local      fileName=$(RGISdirectoryPath            "" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}")
	local      fileName=$(echo ${fileName} | sed "s:/::" | sed "s:/:_:g" )
	local      fileName=${fileName%_*}
	if [ "${rgisDirectory}" == "" ]
	then
		echo ""
		return 1
	fi
	if [[ "${tStepType}" == "static" ]]
	then
		local tStepType="Static"
		local tStepStr=""
	fi

    case "${__RGISarchiveFormat}" in
        (gzipped)
            echo "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}.gz"
        ;;
        (netcdf)
            echo "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.nc"
        ;;
        (*)
        	echo "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}"
        ;;
    esac
}

function RGISfile ()
{
	local      archive="${1}"; shift
	local       domain="${1}"; shift
	local     variable="${1}"; shift
	local      product="${1}"; shift
	local   resolution="${1}"; shift
	local    tStepType="${1}"; shift
	local        tStep=$(echo "${1}" | tr "[A-Z]" "[a-z]"); shift
	local    timeRange="${1}"; shift

	USAGE="Usage: RGISfile <archive> <domain> <variable> <product> <resolution> <tstep type> <tstep> [time range]"
    if [[ "${resolution}" == "" ]]; then echo "${USAGE}"; return 1; fi

	case "${tStep}" in
		(hourly)
			local tStepStr="h"
		;;
		(3hourly)
			local tStepStr="3h"
		;;
		(6hourly)
			local tStepStr="6h"
		;;
		(daily)
			local tStepStr="d"
		;;
		(monthly)
			local tStepStr="m"
		;;
		(annual)
			local tStepStr="a"
		;;
		("")
			local tStepStr=""
		;;
		(*)
			echo "Unknown time step ${tStep}" > /dev/stderr
			return 1
		;;
	esac

    local extension=$(RGISfileExtension "${variable}")

	local rgisDirectory=$(RGISdirectory      "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}")
	local      fileName=$(_RGISresolutionDir "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" | sed "s:/:_:g" )
	if [ "${rgisDirectory}" == "" ]
	then
		echo ""
		return 1
	fi
	if [[ "${tStepType}" == "static" ]]
	then
		local tStepType="Static"
		local tStepStr=""
	fi

	if [[ "${timeRange}" == "xxxx" ]]
	then
		 ncPattern="${rgisDirectory}/${fileName}_${tStepStr}${tStepType}????.nc"
		gdbPattern="${rgisDirectory}/${fileName}_${tStepStr}${tStepType}????.${extension}"
		 gzPattern="${rgisDirectory}/${fileName}_${tStepStr}${tStepType}????.${extension}.gz"

		[ "$(ls -1  ${ncPattern} 2> /dev/null)" != "" ] && { echo  "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}xxxx.nc";              return 0; }
		[ "$(ls -1 ${gdbPattern} 2> /dev/null)" != "" ] && { echo  "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}xxxx.${extension}";    return 0; }
		[ "$(ls -1  ${gzPattern} 2> /dev/null)" != "" ] && { echo  "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}xxxx.${extension}.gz"; return 0; }
	else
   	[ -e "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.nc" ]              && { echo    "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.nc";              return 0;}
   	[ -e "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}.gz" ] && { echo    "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}.gz"; return 0;}
   	[ -e "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}" ]    && { echo    "${rgisDirectory}/${fileName}_${tStepStr}${tStepType}${timeRange}.${extension}";    return 0;}
   fi
   echo ""
   return 1
}

function RGIStitle ()
{
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local  tStepType="${1}"; shift
	local      tStep="${1}"; shift
	local  timeRange="${1}"; shift
	local    version="${1}"; shift

	USAGE="Usage: RGIStitle <archive> <domain> <variable> <product> <resolution> <tstep type> <tstep> <time range> <version>"
    if [[ "${endYear}" == "" ]]; then echo "${USAGE}"; return 1; fi

	local variableFullName=$(_RGISlookupFullName "${variable}")

	case "$(echo "${tStepType}" | tr "[A-Z]" "[a-z]")" in
	    (static)
		    local tStepString=""
		;;
	    (lt|ts)
    		case "$(echo "${tStep}" | tr "[A-Z]" "[a-z]")" in
			    (hourly)
				    local tStepStr="Hourly"
			    ;;
			    (3hourly)
				    local tStepStr="3Hourly"
			    ;;
			    (6hourly)
				    local tStepStr="6Hourly"
			    ;;
			    (daily)
				    local tStepStr="Daily"
			    ;;
			    (monthly)
				    local tStepStr="Monthly"
			    ;;
			    (annual)
				    local tStepStr="Annual"
			    ;;
			    (*)
				    echo "Unknown time step ${tStep}"  > /dev/stderr
			    ;;
    		esac
		    if [[ "${timeRange}" == "" ]]
		    then
			    local tStepString=", ${tStepStr}$(echo "${tStepType}" | tr "[a-z]" "[A-Z]")"
		    else
			    local tStepString=", ${tStepStr}$(echo "${tStepType}" | tr "[a-z]" "[A-Z]"), ${timeRange}"
		    fi
        ;;
    	(*)
    	    echo "Unknown time step type"  > /dev/stderr
    	;;
    esac
    if [[ "${version}" == "" ]]
    then
    	echo "${domain}, ${product} ${variableFullName} (${resolution}${tStepString})"
    else
    	echo "${domain}, ${product} ${variableFullName} (${resolution}${tStepString}) V${version}"
    fi
	return 0
}

function RGISAppend ()
{
    local    archive="${1}"; shift
    local     domain="${1}"; shift
    local   variable="${1}"; shift
    local    product="${1}"; shift
    local resolution="${1}"; shift
    local    version="${1}"; shift
    local  startyear="${1}"; shift
	local    endyear="${1}"; shift
	local      tStep="${1}"; shift

	USAGE="Usage: RGISSAppend <archive> <domain> <variable> <product> <resolution> <version> <start year> <end year> <time step>"
    if [[ "${tStep}" == "" ]]; then echo "${USAGE}"; return 1; fi

 	local      files=""
	local  separator=" "

	for (( year = ${startyear}; year <= ${endyear}; ++year ))
	do
		local  filename=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${tStep}" "${year}")
		local     files="${files}${separator}${filename}"
		local separator=" "
	done

	local filename=$(RGISfile  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${tStep}" "${startyear}-${endyear}")
	local    title=$(RGIStitle              "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${tStep}" "${startyear}-${endyear}" "${version}")
	local  subject=$(RGISlookupSubject  "${variable}")
    local shadeset=$(RGISlookupShadeset "${variable}")

	grdAppendLayers -t "${title}" -d "${domain}" -u "${subject}" -v "${version}" -o "${filename}" ${files}
}

function RGISsetHeader ()
{
	local      archive="${1}"; shift
	local       domain="${1}"; shift
	local     variable="${1}"; shift
	local      product="${1}"; shift
	local   resolution="${1}"; shift
	local    tStepType="${1}"; shift
	local        tStep="${1}"; shift
	local    timeRange="${1}"; shift
	local      version="${1}"; shift
	local        title="${1}"; shift
	local      comment="${1}"; shift
	local     citation="${1}"; shift
	local  institution="${1}"; shift
	local   sourceInst="${1}"; shift
	local sourcePerson="${1}"; shift

	USAGE="Usage: RGISsetHeader <archive> <domain> <variable> <product> <resolution> <tstep type> <tstep> <time range> <version> <title> <comment> <citation> <institution> <source inst> <source person>"
    if [[ "${endYear}" == "" ]]; then echo "${USAGE}"; return 1; fi

	if [[ "$(RGIScase "lower" "${tStepType}")" == "static" ]]
	then
	    local rgisFile="$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}")"
	elif [[     "${tStep}" == "" ]]
	then
	    local rgisFile="$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "Annual")"
	elif [[ "${timeRange}" == "" ]]
	then
	    local rgisFile="$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}")"
	else
	    local rgisFile="$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}" "${timeRange}")"
	fi

	if [[ "${rgisFile}" == "" ]]
	then
		echo "Missing ${rgisFile} in RGISsetHeader"
		return 0
	fi
	if [[ "$(which finger)" == "" ]]
	then
		local person="Annonymous"
	else
		local person=$(finger $(env | grep "LOGNAME" | sed "s:LOGNAME=::") | grep Name | sed -e "s|.*Name: ||")
	fi

	if [[ "${tStepType}"    == "" ]]; then local tStepType="static";  fi
	if [[ "${version}"      == "" ]]; then local   version="pre0.01"; fi

	if [[ "${title}"        == "" ]]; then local        title="$(RGIStitle "${domain}" "${variable}" "${product}" "${resolution}" "${tStepType}" "${tStep}" "${timeRange}" "${version}")"; fi
	if [[ "${comment}"      == "" ]]; then local      comment="${domain} $(_RGISlookupFullName "${variable}") from ${product} at ${resolution}"; fi
	if [[ "${citation}"     == "" ]]; then local     citation="Pirated ${variable} from ${product}"; fi
	if [[ "${institution}"  == "" ]]; then local  institution="Advanced Science Research Center at the Graduate Center, CUNY"; fi
	if [[ "${sourceInst}"   == "" ]]; then local   sourceInst="City College of New York"; fi
	if [[ "${sourcePerson}" == "" ]]; then local sourcePerson="${person}"; fi
	setHeader  -t "${title}" -d "${domain}" -u "$(RGISlookupSubject "${variable}")" -y "on" -c "${comment}" -i "${citation}" -n "${institution}" -o "${sourceInst}" -p "${sourcePerson}" -v "${version}" "${rgisFile}" "${rgisFile}"
}

function RGISAggregateTS ()
{
	local    archive="${1}"; shift
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local    version="${1}"; shift
	local  startyear="${1}"; shift
	local    endyear="${1}"; shift
	local  fromtStep="${1}"; shift
	local    totStep="${1}"; shift

	USAGE="Usage: RGISAggregateTS <archive> <domain> <variable> <product> <resolution> <version> <start year> <end year> <from timestep> <to timestep>"
    if [[ "${totStep}" == "" ]]; then echo "${USAGE}"; return 1; fi

   local     files=""
   local separator=""

   for ((year = ${startyear}; year <= ${endyear} ; ++year))
   do
      local fromFile=$(RGISfile     "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${fromtStep}" "${year}")
      local   toFile=$(RGISfilePath "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${totStep}"   "${year}")
      local    title=$(RGIStitle                 "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${totStep}"   "${year}" "${version}")
      local  subject=$(RGISlookupSubject    "${variable}")
      local shadeset=$(RGISlookupShadeset   "${variable}")
      local   method=$(RGISlookupAggrMethod "${variable}")

      [ -e "${toFile%/*}" ] || mkdir -p "${toFile%/*}"
      grdTSAggr -a "${method}" -e "$(RGISlookupTimeStep ${totStep})" -t "${title}" -d "${domain}" -u "${subject}" -s "${shadeset}" "${fromFile}" "${toFile}" || return 1
   done
}

RGISclimatology ()
{
	local    archive="${1}"; shift
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local   timeStep="$(echo "${1}" | tr "[A-Z]" "[a-z]")"; shift
 	local  startYear="${1}"; shift
	local    endYear="${1}"; shift

	USAGE="Usage: RGISclimatology <archive> <domain> <variable> <product> <resolution> [annual|monthly] [start year] [end year]"
    if [[ "${timeStep}" == "" ]]; then echo "${USAGE}"; return 1; fi

	if [[ "${startYear}" == "" ]]
	then
		if [[ "${endYear}" == "" ]]
		then
			local tsFiles=""
			local     sep=""
			for fileName in $(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${timeStep}" "*")
			do
				local tsFiles="${tsFiles}${sep}${fileName}"
				local     sep=" "
			done
			local range=""
		else
			local tsFiles=""
			local     sep=""
			local lastFile="$(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${timeStep}" "${endYear}")"
			for fileName in $(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${timeStep}" "*")
			do
				if [[ ${fileName} == ${lastFile} ]]
				then
					break;
				else
					local tsFiles="${tsFiles}${sep}${fileName}"
					local     sep=" "
				fi
			done
			local tsFiles="${tsFiles}${sep}${fileName}"
			local range="year-${endYear}"
		fi
	elif [[ "${endYear}" == "" ]]
	then
		local tsFiles=""
		local     sep=""
		for (( year = ${startYear}; ; ++year ))
			do
			local fileName="$(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${timeStep}" "${year}")"
			if [ -e ${fileName} ]
			then
				local tsFiles="${tsFiles}${sep}${fileName}"
				local     sep=" "
			else
				break;
			fi
			done
			local range="${startYear}-year"
	else
		local tsFiles=""
		local     sep=""
		for (( year = ${startYear}; year <= ${endYear}; ++year ))
			do
			local fileName="$(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS" "${timeStep}" "${year}")"
			if [ -e ${fileName} ]
			then
				local tsFiles="${tsFiles}${sep}${fileName}"
				local     sep=" "
			else
				break;
			fi
		done
		local range="${startYear}-${endYear}"
	fi

	case "${timeStep}" in
	(annual)
		local     nStep="1"
		local tStepUnit="year"
	;;
	(monthly)
		local     nStep="12"
		local tStepUnit="month"
	;;
	(*)
		echo "Invalid time step: ${timeStep}"
		exit 1
	;;
	esac

	local fileName="$(RGISfilePath  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LT" "${timeStep}" "${range}")"
	local    title="$(RGIStitle                  "${domain}" "${variable}" "${product}" "${resolution}" "LT" "${timeStep}" "${range}")"
	[ -e "${fileName%/*}"  ] || mkdir -p "${fileName%/*}"
	grdAppendLayers ${tsFiles} |\
	grdCycleMean -n "${nStep}" -t "${title}" -u "$(RGISlookupSubject "${variable}")" -d "${domain}" -s "$(RGISlookupShadeset "${variable}")" |\
	grdDateLayers -e "${tStepUnit}" - "${fileName}"
}

function RGISCellStats ()
{
	local    archive="${1}"; shift
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local    version="${1}"; shift
 	local  startyear="${1}"; shift
	local    endyear="${1}"; shift

	USAGE="Usage: RGISCellStats <archive> <domain> <variable> <product> <resolution> <version> <start year> <end year>"
    if [[ "${endYear}" == "" ]]; then echo "${USAGE}"; return 1; fi

	local  statsDir=$(RGISdirectory  "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "Stats" "annual")
	local    tsFile=$(RGISfile       "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS"    "annual" "${startyear}-${endyear}")
	local statsFile=$(RGISfile       "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "Stats" "annual" "${startyear}-${endyear}")
	local     title=$(RGIStitle                   "${domain}" "${variable}" "${product}" "${resolution}" "Stats" "annual" "${startyear}-${endyear}" "${version}")
	local   subject=$(RGISlookupSubject "${variable}")

	[ -e "${statsDir}" ] || mkdir -p "${statsDir}" || return 1
	grdCellStats    -t "${title}" -u "${variable}" -d "${domain}" -v "${version}" "${tsFile}" "${statsFile}" || return 1

	local  annualLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LT"      "annual"  "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LT"      "annual"  "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "avg" -l "avg" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${annualLTfile}" || return 1

	local     maxLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTmax"    "annual"  "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTmax"    "annual"  "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Maximum" -l "Maximum" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${maxLTfile}" || return 1

	local     minLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTmin"    "annual"  "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTmin"    "annual"  "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Minimum" -l "Minimum" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${minLTfile}" || return 1

	local   rangeLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTrange"  "annual" "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTrange"  "annual" "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "Range"   -l "Range"   -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${rangeLTfile}" || return 1

	local   slopeLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTslope"  "annual" "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTslope"  "annual" "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "SigSlopeB1" -l "SigSlopeB1" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${slopeLTfile}" || return 1

	local  stdDevLTfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "LTstdDev" "annual" "${startyear}-${endyear}")
	local         title=$(RGIStitle             "${domain}" "${variable}" "${product}" "${resolution}" "LTstdDev" "annual" "${startyear}-${endyear}" "${version}")
	grdExtractLayers -t "${title}" -f "StdDev" -l "StdDev" -d "${domain}" -u "$(RGISlookupSubject ${variable})" "${statsFile}" |\
	grdDateLayers -e "year" - "${stdDevLTfile}" || return 1

#	rm "${statsFile}"
	return 0
}

function RGISStatistics ()
{
	local    archive="${1}"; shift
	local     domain="${1}"; shift
	local   variable="${1}"; shift
	local    product="${1}"; shift
	local resolution="${1}"; shift
	local    version="${1}"; shift
	local  startyear="${1}"; shift
	local    endyear="${1}"; shift

	USAGE="Usage: RGISStatistics <archive> <domain> <variable> <product> <resolution> <version> <start year> <end year>"
    if [[ "${endYear}" == "" ]]; then echo "${USAGE}"; return 1; fi

	RGISAppend "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}" "monthly" || return 1
	RGISAppend "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}" "annual"  || return 1

	RGISClimatology "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}"      || return 1
	RGISCellStats   "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "${version}" "${startyear}" "${endyear}"      || return 1
	local  annualTSfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS"    "annual"  "${startyear}-${endyear}")
	local monthlyTSfile=$(RGISfile "${archive}" "${domain}" "${variable}" "${product}" "${resolution}" "TS"    "monthly" "${startyear}-${endyear}")
	[ -e  "${annualTSfile}" ] && rm  "${annualTSfile}"
	[ -e "${monthlyTSfile}" ] && rm "${monthlyTSfile}"
	return 0
}

if [[ $0 == "${BASH_SOURCE}" ]]
then
    if (( $# > 1))
    then
        FUNCTION="$1"; shift
        ARGUMENTS="$@"
        ${FUNCTION} ${ARGUMENTS}
    fi
fi