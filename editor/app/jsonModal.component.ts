import {
	Component,
	Input,
	ViewEncapsulation
} from '@angular/core';

@Component({
	selector: 'json-modal',
	template: `
<!-- Trigger/Open The Modal -->
<button  (click)='onOpen(true)'>{{object|json}}</button>

<!-- The Modal -->
<div class='modal' *ngIf='!closed' (click)='onOpen(false)'>
	<!-- Modal content -->
	<div class='modal-content' (click)='onNop($event)'>
		<span class='close' (click)='onOpen(false)'>&times;</span>
		<textarea class='jsontextarea' [ngModel]='object|json'></textarea>
		<button  (click)='onSave()'>Save</button>
	</div>
</div>
`,
	host: {
		'class': 'input-value'
	},
	styles: [
`
button {
	width:100px;
	white-space:nowrap;
	overflow: hidden;
}
/* The Modal (background) */
.modal {
	display: block; /* Hidden by default */
	position: fixed; /* Stay in place */
	z-index: 2; /* Sit on top */
	padding-top: 100px; /* Location of the box */
	left: 0;
	top: 0;
	width: 100%; /* Full width */
	height: 100%; /* Full height */
	overflow: auto; /* Enable scroll if needed */
	background-color: rgb(0,0,0); /* Fallback color */
	background-color: rgba(0,0,0,0.6); /* Black w/ opacity */
}

/* Modal Content */
.modal-content {
	background-color: #fefefe;
	z-index: 1; /* Sit on top */
	margin: auto;
	padding: 20px;
	border: 1px solid #888;
	width: 80%;
	height: 80%;
}

/* The Close Button */
.close {
	color: #aaaaaa;
	float: right;
	font-size: 28px;
	font-weight: bold;
}

.close:hover,
.close:focus {
	color: #000;
	text-decoration: none;
	cursor: pointer;
}

.jsontextarea {
	width: 90%;
	height: 90%;
}
`
	],
	encapsulation: ViewEncapsulation.Emulated
})
export class JsonModal {
	@Input() public object: Object;
	private closed: boolean = true;

	onOpen(value: boolean): void {
		this.closed = !value;
		// this.json = JSON.stringify(this.object);
	}
	onNop(event: any) {
		event.stopPropagation();
	}
	onSave(json): void {
		// TODO this.object = JSON.parse(json);
		// console.log(this.json);
		this.onOpen(false);
		// this.object = {};
	}
}
