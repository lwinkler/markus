import {
	Component,
	Input,
	ViewEncapsulation
} from '@angular/core';
import {ModuleInput}  from './moduleInput';

@Component({
	selector: 'input-value',
	template: `
<span type='text' *ngIf='input!==undefined&&input.connected'>(connected)</span>
<select *ngIf='input!==undefined&&description.enum' type='checkbox' [(ngModel)]='input.value'>
	<option *ngFor='let val of (description.range.allowed||description.enum)' [value]='val'>{{val}}</option>
</select>

<div [ngSwitch]='description.class' *ngIf='input!==undefined&&input.value!==undefined&&!description.enum'>
	<input *ngSwitchCase='"ParameterFloat"'        type='number' min='{{description.range.min}}' max='{{description.range.max}}' step='0.01' [(ngModel)]='input.value' (ngModelChange)='castFloat()' size='10'/>
	<input *ngSwitchCase='"ParameterDouble"'       type='number' min='{{description.range.min}}' max='{{description.range.max}}' step='0.01' [(ngModel)]='input.value' (ngModelChange)='castFloat()' size='10'/>
	<input *ngSwitchCase='"ParameterInt"'          type='number' min='{{description.range.min}}' max='{{description.range.max}}' step='1'    [(ngModel)]='input.value' (ngModelChange)='castInt()' size='10'/>
	<input *ngSwitchCase='"ParameterUInt"' type='number' min='{{description.range.min}}' max='{{description.range.max}}' step='1'    [(ngModel)]='input.value' (ngModelChange)='castInt()' size='10'/>
	<input *ngSwitchCase='"ParameterBool"' type='checkbox' [(ngModel)]='input.value'/>
	<input *ngSwitchCase='"ParameterString"' type='string' [(ngModel)]='input.value' size='10'/>
	<json-modal *ngSwitchDefault [(object)]='input.value' (save)='input.value=$object' ></json-modal>
</div>
`,
	host: {
		'class': 'input-value'
	},
	styles: [
`
span {
	width: 100%;
}
input {
	width: 100%;
}
select {
	width: 100%;
}
`
	],
	encapsulation: ViewEncapsulation.Emulated
})
export class InputValue {
	@Input() private input: ModuleInput;
	@Input() public  description: any;
	castFloat(): any {
		this.input.value = parseFloat(this.input.value);
	}
	castInt(): any {
		this.input.value = parseInt(this.input.value, 10);
	}
}
