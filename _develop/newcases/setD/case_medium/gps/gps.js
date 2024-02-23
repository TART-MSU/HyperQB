

var longitude = 0;
var latitude = 0;


// watch visitor's location, updated every time step
function watchLocation() {
  if (navigator.geolocation) {
    navigator.geolocation.watchPosition(showPosition, handleError);
  } else {
    console.error("Geolocation is not supported by this browser.");
  }
}


function project_zero_Map() {
	map = new google.maps.Map( document.getElementById( 'map' ), {
		center: { lat: 0, lng: 0},
		zoom: 14
	});
}


function project_approx_Map() {
	math.floor(longitude)
	math.floor(latitude)
	map = new google.maps.Map( document.getElementById( 'map' ), {
		center: { lat: latitude, lng: longitude},
		zoom: 14
	});
}

function showPosition(position) {
  console.log(`Latitude: ${position.coords.latitude}, longitude: ${position.coords.longitude}`);
//   latiitude = {position.coords.latitude}
//   longitude = {position.coords.longitude}

  // everytime when showPosition is called, we check the Cookie. 
  if(readCookie("GPS_tracking_enabled")) {
  	// if the Cookie is OK, we round the coordinate
  	project_approx_Map()
  }
  else {
  	// else set coordinate to null island. (i.e., (0,0))
  	project_zero_Map()
  }
}









