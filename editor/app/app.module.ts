import {NgModule, ErrorHandler} from '@angular/core';
import {BrowserModule} from '@angular/platform-browser';
import {FormsModule} from '@angular/forms';
import {HttpModule} from '@angular/http';
import {AppComponent} from './app.component';
import {TopBarComponent} from './topBar.component';
import {SideBarComponent} from './sideBar.component';
import {InputSet} from './inputSet.component';
import {InputValue} from './inputValue.component';
import {JsonModal} from './jsonModal.component';
import {ProjectWindowComponent} from './projectWindow.component';
import {ModuleComponent} from './module.component';

class MyErrorHandler implements ErrorHandler {
	handleError(error: any) {
		// do something with the exception
		if(error.originalError)
			console.log(error.originalError);
		console.log(error);
	}
}

@NgModule({
	imports: [
		BrowserModule,
		FormsModule,
		HttpModule
	],
	declarations: [
		AppComponent,
		TopBarComponent,
		SideBarComponent,
		ProjectWindowComponent,
		ModuleComponent,
		InputSet,
		InputValue,
		JsonModal
	],
	bootstrap: [AppComponent],
	providers: [{provide: ErrorHandler, useClass: MyErrorHandler}]
})
export class AppModule {}
