import {
	Component,
	Input,
	ViewEncapsulation
} from '@angular/core';
import {ModuleInput}  from './moduleInput';
import {Utils} from './utils';

@Component({
	selector: 'input-value',
	template: `
<input type='checkbox' (change)='activate($event)' value='input!==undefined'/>
<span type='text' *ngIf='input!==undefined&&input.connected'>(connected)</span>
<select *ngIf='input!==undefined&&description.range.allowed' type='checkbox' [(ngModel)]='input.value'>
	<option *ngFor='let val of description.range.allowed' [value]='val'>{{val}}</option>
</select>

<div [ngSwitch]='description.type' *ngIf='input!==undefined&&input.value!==undefined&&!description.range.allowed'>
	<input *ngSwitchCase='"float"'        type='number' min='{{description.range.min}}' max='{{description.range.max}}' step='0.01' [(ngModel)]='input.value' (ngModelChange)='castFloat()' size='10'/>
	<input *ngSwitchCase='"double"'       type='number' min='{{description.range.min}}' max='{{description.range.max}}' step='0.01' [(ngModel)]='input.value' (ngModelChange)='castFloat()' size='10'/>
	<input *ngSwitchCase='"int"'          type='number' min='{{description.range.min}}' max='{{description.range.max}}' step='1'    [(ngModel)]='input.value' (ngModelChange)='castInt()' size='10'/>
	<input *ngSwitchCase='"unsigned int"' type='number' min='{{description.range.min}}' max='{{description.range.max}}' step='1'    [(ngModel)]='input.value' (ngModelChange)='castInt()' size='10'/>
	<input *ngSwitchCase='"bool"' type='checkbox' [(ngModel)]='input.value'/>
	<input *ngSwitchCase='"string"' type='string' [(ngModel)]='input.value' size='10'/>
	<span *ngSwitchDefault>unknown type</span>
</div>
<span *ngIf='input===undefined'>
	(unset)
</span>
`,
	host: {
		'class': 'input-value'
	},
	styles: [
`.input-value {
	white-space:nowrap;
}
span {
	white-space:nowrap;
}
div {
	white-space:nowrap;
}
select {
	width: 100px;
	display: inline-block
}
`
	],
	encapsulation: ViewEncapsulation.Emulated
})
export class InputValue {
	@Input()
	input: ModuleInput;
	@Input()
	description: any;
	@Input()
	inputs: Array<any>;
	activate($event: any): void {
		if($event.target.checked) {
			this.input = Utils.findByNameAndAdd(this.inputs, this.description.name);
			this.input.value = this.description.default;
		} else {
			Utils.removeByName(this.inputs, this.description.name);
			this.input = undefined;
		}
	}
	castFloat(): any {
		this.input.value = parseFloat(this.input.value);
	}
	castInt(): any {
		this.input.value = parseInt(this.input.value, 10);
	}
}
