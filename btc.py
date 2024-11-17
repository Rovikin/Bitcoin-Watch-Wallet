import requests
from datetime import datetime, timezone, timedelta

def get_bitcoin_address_details(address):
    url = f'https://api.blockcypher.com/v1/btc/main/addrs/{address}?includeSubaccounts=true&includeUnconfirmed=true'
    response = requests.get(url)
    return response.json() if response.status_code == 200 else {'error': 'Unable to fetch data'}

def get_btc_to_idr_exchange_rate():
    url = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=idr"
    response = requests.get(url)
    data = response.json()
    return data.get('bitcoin', {}).get('idr', None) if response.status_code == 200 else None

def get_btc_to_usd_price():
    url = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd"
    response = requests.get(url)
    data = response.json()
    return data.get('bitcoin', {}).get('usd', None) if response.status_code == 200 else None

def get_fear_and_greed_index():
    url = "https://api.alternative.me/fng/"
    response = requests.get(url)
    data = response.json()
    return int(data['data'][0].get('value', '0')) if response.status_code == 200 else None

def determine_status(index_value):
    if index_value < 25:
        return "Extreme Fear"
    elif 25 <= index_value < 50:
        return "Fear"
    elif 50 <= index_value < 75:
        return "Greed"
    else:
        return "Extreme Greed"

def get_mempool_fee_rate():
    url = "https://mempool.space/api/v1/fees/recommended"
    response = requests.get(url)
    data = response.json()
    return {
        'slow': data.get('hourFee', 'N/A'),
        'medium': data.get('halfHourFee', 'N/A'),
        'fast': data.get('fastestFee', 'N/A')
    } if response.status_code == 200 else None

def format_rupiah(amount_satoshi, btc_to_idr_rate):
    amount_idr = (amount_satoshi / 100_000_000) * btc_to_idr_rate
    return f"Rp. {amount_idr:,.0f}".replace(",", ".")

def main():
    try:
        with open("wallet.txt", "r") as file:
            addresses = file.read().splitlines()
    except FileNotFoundError:
        print("File 'wallet' tidak ditemukan.")
        return

    total_balance_satoshi = sum(
        get_bitcoin_address_details(address).get('final_balance', 0) for address in addresses
    )

    btc_to_idr_rate = get_btc_to_idr_exchange_rate()
    btc_to_usd_price = get_btc_to_usd_price()
    fee_rate = get_mempool_fee_rate()
    fear_and_greed_index = get_fear_and_greed_index()

    if btc_to_idr_rate is None or btc_to_usd_price is None:
        print("Tidak dapat memperoleh nilai tukar atau harga BTC.")
        return

    formatted_balance = f"{total_balance_satoshi:,}".replace(",", ".")
    balance_rupiah = format_rupiah(total_balance_satoshi, btc_to_idr_rate)

    print("\n----------------------------------")
    print("~Balance~")
    print(f"{formatted_balance} sat")
    print(f"{balance_rupiah}")

    if fear_and_greed_index is not None:
        status = determine_status(fear_and_greed_index)
        print("----------------------------------")
        print("~Fear & Greed Index~")
        print(f"Index: {fear_and_greed_index}")
        print(f"Status: {status}")

    if fee_rate:
        print("----------------------------------")
        print("~Mempool fee rate~")
        print(f"Slow: {fee_rate['slow']} sat/vB")
        print(f"Medium: {fee_rate['medium']} sat/vB")
        print(f"Fast: {fee_rate['fast']} sat/vB")

    print("----------------------------------")
    print("~Price~")
    print(f"BTC: $ {btc_to_usd_price:,.0f}")
    print("----------------------------------")
if __name__ == "__main__":
    main()
