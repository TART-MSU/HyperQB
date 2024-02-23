var platforms = [{
	domain: "https://twitter.com",
	redirect: "/login?redirect_after_login=%2Ffavicon.ico",
	name: "Twitter"
	}, {
	domain: "https://www.facebook.com",
	redirect: "/login.php?next=https%3A%2F%2Fwww.facebook.com%2Ffavicon.ico%3F_rdr%3Dp",
	name: "Facebook"
	}, {
	domain: "https://www.spotify.com",
	redirect: "/en/login/?forward_url=https%3A%2F%2Fwww.spotify.com%2Ffavicon.ico",
	name: "Spotify"
	}, {
	domain: "https://www.academia.edu",
	redirect: "/login?cp=/favicon.ico&cs=www",
	name: "Academia.edu"
	}, {
	domain: "https://github.com",
	redirect: "/login?return_to=https%3A%2F%2Fgithub.com%2Ffavicon.ico%3Fid%3D1",
	name: "Github"
	}, {
	domain: "https://slack.com",
	redirect: "/checkcookie?redir=https%3A%2F%2Fslack.com%2Ffavicon.ico%23",
	name: "Slack"
	}, {
	domain: "https://www.paypal.com",
	redirect: "/signin?returnUri=https://t.paypal.com/ts?v=1.0.0",
	name: "Paypal"
}];
​
​
platforms.forEach(function(network) {
        var img = document.createElement('img');
	img.src = network.domain + network.redirect;
	//Simulates output from img.src load
	var onLoadResult = utils.source((Math.random() >= 0.5), utils.HIGH_LEVEL, "onLoadresult");
	if (onLoadResult){
		// Img is loaded -> User is logged in the network
		img.onload = function() {
		    //callback(network, true);
		}();
	}
	else {
		// Img returns with error -> User is *not* logged in the network
		img.onerror = function() {
		    //callback(network, false);
		}();
	}
	});
​
