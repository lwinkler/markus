import {Component, ViewEncapsulation} from '@angular/core';

@Component({
	selector: 'markus-app',
	template: `
<div class='markus-app'>
	<nav>
		<a routerLink="/editor" routerLinkActive="active">Editor</a>
		<a routerLink="/dbObjectViewer" routerLinkActive="active">Object viewer</a>
	</nav>
	<router-outlet></router-outlet>
</div>
`,
	styles: [`
	nav a + a:before{
		content: " | ";
		padding: 0 10px;
	}
`
	],
	encapsulation: ViewEncapsulation.Emulated
})
export class AppComponent {
}
