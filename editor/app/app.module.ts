import {NgModule, ErrorHandler} from '@angular/core';
import {RouterModule, Routes, ROUTER_DIRECTIVES} from '@angular/router';
import {BrowserModule} from '@angular/platform-browser';
import {FormsModule} from '@angular/forms';
import {HttpModule} from '@angular/http';
import {AppComponent} from './app.component';
import {TopBarComponent} from './topBar.component';
import {SideBarComponent} from './sideBar.component';
import {InputSet} from './inputSet.component';
import {InputValue} from './inputValue.component';
import {MarkusEditor} from './markusEditor.component';
import {DbObjectViewer} from './dbObjectViewer.component';
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

const appRoutes: Routes = [
	{path: 'editor',         component: MarkusEditor},
	{path: 'dbObjectViewer', component: DbObjectViewer}
];

@NgModule({
	imports: [
		RouterModule.forRoot(appRoutes),
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
		JsonModal,
		DbObjectViewer,
		MarkusEditor
	],
	bootstrap: [AppComponent],
	providers: [{provide: ErrorHandler, useClass: MyErrorHandler}]
})
export class AppModule {}
