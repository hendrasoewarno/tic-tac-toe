// To see this in action, run this in a terminal:
//      gp preview $(gp url 8000)

import * as React from "react";
import * as ReactDOM from "react-dom";
import { Api, JsonRpc, RpcError } from 'eosjs';
import { JsSignatureProvider } from 'eosjs/dist/eosjs-jssig';

const rpc = new JsonRpc(''); // nodeos and web server are on same port

interface WelcomeData {
    host?: string;
    opponent?: string;
};

interface PostFormState {
    privateKey: string;
    data: WelcomeData;
    succeed: string;
    error: string;
};

class WelcomeForm extends React.Component<{}, PostFormState> {
    api: Api;

    constructor(props: {}) {
        super(props);
        this.api = new Api({ rpc, signatureProvider: new JsSignatureProvider([]) });
        this.state = {
            privateKey: '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',
            data: {
                host: 'tictactoe',
                opponent: 'bob',
            },
            succeed: '',
            error: '',
        };
    }

    setData(data: WelcomeData) {
        this.setState({ data: { ...this.state.data, ...data } });
    }

    async post() {
        try {
            this.api.signatureProvider = new JsSignatureProvider([this.state.privateKey]);
            const result = await this.api.transact(
                {
                    actions: [{
                        account: 'tictactoe',
                        name: 'welcome',
                        authorization: [{
                            actor: this.state.data.host,
                            permission: 'active',
                        }],
                        data: this.state.data,
                    }]
                }, {
                    blocksBehind: 3,
                    expireSeconds: 30,
                });
            console.log(result);
            this.setState({ succeed:'Ok'});
            this.setState({ error: '' });
        } catch (e) {
            this.setState({succeed: ''});
            if (e.json)
                this.setState({ error: JSON.stringify(e.json, null, 4) });
            else
                this.setState({ error: '' + e });
        }
    }

    render() {
        return <div>
            <table>
                <tbody>
                    <tr>
                        <td>Private Key</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.privateKey}
                            onChange={e => this.setState({ privateKey: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>User</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.host}
                            onChange={e => this.setData({ host: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>Opponent</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.opponent}
                            onChange={e => this.setData({ opponent: e.target.value })}
                        /></td>
                    </tr>
                </tbody>
            </table>
            <br />
            <button onClick={e => this.post()}>Post</button>
            {this.state.succeed && <div>
                <br />
                Succeed:
                <code><pre>{this.state.succeed}</pre></code>
            </div>}            
            {this.state.error && <div>
                <br />
                Error:
                <code><pre>{this.state.error}</pre></code>
            </div>}
        </div>;
    }
}

ReactDOM.render(
    <div>
        <WelcomeForm />
    </div>,
    document.getElementById("body")
);