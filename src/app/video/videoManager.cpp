/*
 *   
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 *  
 */


#include "videoManager.h"

extern "C" {
#include "macGlutfix.h"
}


videoManager::videoManager () {
	
	bHasPixels = false;
	videoPlayer = NULL;
		
}

void videoManager::setup(int webcamW, int webcamH){
		
	// init webcam and set defaultmode
	videoGrabber.initGrabber(webcamW, webcamH);
	setVideoMode(CORRUPT_VIDEOMODE_WEBCAM);
	ofAddListener(ofEvents.windowResized, this, &videoManager::windowResized);
	
}
void videoManager::update(){
	
	// switch videoMode and update 
	
	switch (videoMode) {
		
		case CORRUPT_VIDEOMODE_WEBCAM:
			
			
			
			videoGrabber.grabFrame();
			
			if ( videoGrabber.isFrameNew() ) {
				
				pixels = videoGrabber.getPixels();
				bHasPixels = true;
				
			}	
			
			break;
			
		case CORRUPT_VIDEOMODE_MOVIE:
		case CORRUPT_VIDEOMODE_IMGSEARCH:
			
			if(!videoPlayer) return;
			if(videoPlayer->isLoaded() ) {
				
				// don't know why play is needed there - sometimes It's just stucked
				videoPlayer->play();
				videoPlayer->update();
				pixels = videoPlayer->getPixels();
				moviePos = getMoviePosition(); // update for GUI
				bHasPixels = true;
			}
			
			break;
			
		case CORRUPT_VIDEOMODE_DESKTOP:
			
			// thanks to zachary for this!!
			pixels = pixelsBelowWindow(ofGetWindowPositionX(),ofGetWindowPositionY(),ofGetWidth(), ofGetHeight());
			
			for (int i = 0; i < ofGetWidth()*ofGetHeight(); i++){  
				
				unsigned char r1 = pixels[i*4]; // mem A  
				
				pixels[i*4]   = pixels[i*4+1];   
				pixels[i*4+1] = pixels[i*4+2];   
				pixels[i*4+2] = pixels[i*4+3];   
				pixels[i*4+3] = r1;   
			}  
			
			bHasPixels = true;
			
			break;
			
			
	}
	
	
}


void videoManager::videoManager::draw(int x, int y){
	
}

void videoManager::draw(int x, int y, float width, float height){
	
	switch (videoMode) {
				
		case CORRUPT_VIDEOMODE_WEBCAM:
			
			videoGrabber.draw(x,y,width,height);
			
			break;
			
		case CORRUPT_VIDEOMODE_MOVIE:
		case CORRUPT_VIDEOMODE_IMGSEARCH:
			videoPlayer->draw(x,y,width,height);
			
			break;
			
		case CORRUPT_VIDEOMODE_DESKTOP:
			
			// ?
			
			break;
			
			
	}
	
}


unsigned char * videoManager::getPixels(){
	
	return pixels;
	
}

void videoManager::stopVideos() {
	
	if(videoPlayer ) videoPlayer->stop();
		
}


void videoManager::setVideoMode(corruptVideoMode mode){
	
	bHasPixels = false;
	
	videoMode = mode;
	
	switch (videoMode) {
			
		case CORRUPT_VIDEOMODE_WEBCAM:
			
			width = videoGrabber.width;
			height = videoGrabber.height;
			colorMode = OF_IMAGE_COLOR;
			break;
			
		case CORRUPT_VIDEOMODE_MOVIE:
			
			loadMovie();
			colorMode = OF_IMAGE_COLOR;
			break;
			
		case CORRUPT_VIDEOMODE_IMGSEARCH:
			colorMode = OF_IMAGE_COLOR;
			break;
			
			
		case CORRUPT_VIDEOMODE_DESKTOP:
			
			width = ofGetWidth();
			height = ofGetHeight();
			colorMode = OF_IMAGE_COLOR_ALPHA;
			break;
			
	}
	
}

int videoManager::getVideoMode() {
	return videoMode;
}



/* for movie mode */

bool videoManager::loadMovie() {
	
	ofxFileDialog dialog;
	
	string filePath = dialog.getStringFromDialog(kDialogFile, "Choose File to Open", NULL);
	
	
	if(filePath.empty()) {
		
		ofLog(OF_LOG_NOTICE, "No file chosen - skip");
		return false;
		
	}
	
	loadMovie(filePath);
	
	return true;
	
}

bool videoManager::loadMovie(string path) {
		
	
	clear();
	
	videoPlayer = new ofVideoPlayer();
	videoPlayer->loadMovie(path);
	
	width = videoPlayer->width;
	height = videoPlayer->height;
	
	
	
	videoPlayer->play();
	bHasPixels = false;
	
	return true;
	
}

void videoManager::setMoviePosition(float pct){
	if(videoPlayer) videoPlayer->setPosition(pct);
}

float videoManager::getMoviePosition(){
	if(!videoPlayer) return 0.0;
	return videoPlayer->getPosition();
}



/* for webcam mode*/
void videoManager::showWebcamSettings(){
	videoGrabber.videoSettings();
}


void videoManager::windowResized(ofResizeEventArgs & e ){
	
	if ( videoMode == CORRUPT_VIDEOMODE_DESKTOP ) {
		width = ofGetWidth();
		height = ofGetHeight();
	}
	
}

void videoManager::clear () {
	
	if(videoPlayer) {
		
		videoPlayer->close();
		delete videoPlayer;
		videoPlayer = NULL;
	}
	
}

void videoManager::exit(){
	
	clear();
	
}
